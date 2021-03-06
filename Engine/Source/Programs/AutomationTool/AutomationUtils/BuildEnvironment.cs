// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Reflection;
using Microsoft.Win32;
using System.Diagnostics;
using Tools.DotNETCommon;
using UnrealBuildTool;
using System.Text.RegularExpressions;

namespace AutomationTool
{
	/// <summary>
	/// Defines the environment variable names that will be used to setup the environemt.
	/// </summary>
	static class EnvVarNames
	{
		// Command Environment
		public const string LocalRoot = "uebp_LOCAL_ROOT";
		public const string LogFolder = "uebp_LogFolder";
		public const string CSVFile = "uebp_CSVFile";
		public const string EngineSavedFolder = "uebp_EngineSavedFolder";
		public const string MacMallocNanoZone = "MallocNanoZone";
		public const string DisableStartupMutex = "uebp_UATMutexNoWait";
		public const string IsChildInstance = "uebp_UATChildInstance";

		// Perforce Environment
		public const string P4Port = "uebp_PORT";
		public const string ClientRoot = "uebp_CLIENT_ROOT";
		public const string Changelist = "uebp_CL";
		public const string CodeChangelist = "uebp_CodeCL";
		public const string User = "uebp_USER";
		public const string Client = "uebp_CLIENT";
		public const string BuildRootP4 = "uebp_BuildRoot_P4";
		public const string BuildRootEscaped = "uebp_BuildRoot_Escaped";
		public const string P4Password = "uebp_PASS";
	}


	/// <summary>
	/// Environment to allow access to commonly used environment variables.
	/// </summary>
	public class CommandEnvironment
	{
		/// <summary>
		/// Path to a file we know to always exist under the UE4 root directory.
		/// </summary>
		public static readonly string KnownFileRelativeToRoot = @"Engine/Config/BaseEngine.ini";

		#region Command Environment properties

		public string LocalRoot { get; protected set; }
		public string EngineSavedFolder { get; protected set; }
		public string LogFolder { get; protected set; }
        public string CSVFile { get; protected set; }
		public string RobocopyExe { get; protected set; }
		public string MountExe { get; protected set; }
		public string CmdExe { get; protected set; }
		public string UATExe { get; protected set; }		
		public string TimestampAsString { get; protected set; }
		public bool HasCapabilityToCompile { get; protected set; }
		public string MsBuildExe { get; protected set; }
		public string MallocNanoZone { get; protected set; }
		public bool IsChildInstance { get; protected set; }

		#endregion

		internal CommandEnvironment()
		{
			InitEnvironment();
		}

		/// <summary>
		/// Sets the location of the exe.
		/// </summary>
		protected void SetUATLocation()
		{
			if (String.IsNullOrEmpty(UATExe))
			{
				UATExe = Assembly.GetEntryAssembly().GetOriginalLocation();
			}
			if (!CommandUtils.FileExists(UATExe))
			{
				throw new AutomationException("Could not find AutomationTool.exe. Reflection indicated it was here: {0}", UATExe);
			}
		}

		/// <summary>
		/// Sets the location of the AutomationTool/Saved directory
		/// </summary>
		protected void SetUATSavedPath()
		{
			var LocalRootPath = CommandUtils.GetEnvVar(EnvVarNames.LocalRoot);
			var SavedPath = CommandUtils.CombinePaths(PathSeparator.Slash, LocalRootPath, "Engine", "Programs", "AutomationTool", "Saved");
			CommandUtils.SetEnvVar(EnvVarNames.EngineSavedFolder, SavedPath);
		}

		/// <summary>
		/// Initializes the environement.
		/// </summary>
		protected virtual void InitEnvironment()
		{
			SetUATLocation();

			LocalRoot = CommandUtils.GetEnvVar(EnvVarNames.LocalRoot);
			if (String.IsNullOrEmpty(CommandUtils.GetEnvVar(EnvVarNames.EngineSavedFolder)))
			{
				SetUATSavedPath();
			}

			if (LocalRoot.EndsWith(":"))
			{
				LocalRoot += Path.DirectorySeparatorChar;
			}

			EngineSavedFolder = CommandUtils.GetEnvVar(EnvVarNames.EngineSavedFolder);
            CSVFile = CommandUtils.GetEnvVar(EnvVarNames.CSVFile);            
			LogFolder = CommandUtils.GetEnvVar(EnvVarNames.LogFolder);
			RobocopyExe = GetSystemExePath("robocopy.exe");
			MountExe = GetSystemExePath("mount.exe");
			CmdExe = Utils.IsRunningOnMono ? "/bin/sh" : GetSystemExePath("cmd.exe");
			MallocNanoZone = "0";
			CommandUtils.SetEnvVar(EnvVarNames.MacMallocNanoZone, MallocNanoZone);
			if (String.IsNullOrEmpty(LogFolder))
			{
				throw new AutomationException("Environment is not set up correctly: LogFolder is not set!");
			}

			if (String.IsNullOrEmpty(LocalRoot))
			{
				throw new AutomationException("Environment is not set up correctly: LocalRoot is not set!");
			}

			if (String.IsNullOrEmpty(EngineSavedFolder))
			{
				throw new AutomationException("Environment is not set up correctly: EngineSavedFolder is not set!");
			}

			int IsChildInstanceInt;
			int.TryParse(CommandUtils.GetEnvVar("uebp_UATChildInstance", "0"), out IsChildInstanceInt);
			IsChildInstance = (IsChildInstanceInt != 0);

			// Make sure that the log folder exists
			var LogFolderInfo = new DirectoryInfo(LogFolder);
			if (!LogFolderInfo.Exists)
			{
				LogFolderInfo.Create();
			}

			// Setup the timestamp string
			DateTime LocalTime = DateTime.Now;

			string TimeStamp = LocalTime.Year + "-"
						+ LocalTime.Month.ToString("00") + "-"
						+ LocalTime.Day.ToString("00") + "_"
						+ LocalTime.Hour.ToString("00") + "."
						+ LocalTime.Minute.ToString("00") + "."
						+ LocalTime.Second.ToString("00");

			TimestampAsString = TimeStamp;

			SetupBuildEnvironment();

			LogSettings();
		}

		/// <summary>
		/// Returns the path to an executable in the System Directory.
		/// To help support running 32-bit assemblies on a 64-bit operating system, if the executable
		/// can't be found in System32, we also search Sysnative.
		/// </summary>
		/// <param name="ExeName">The name of the executable to find</param>
		/// <returns>The path to the executable within the system folder</returns>
		string GetSystemExePath(string ExeName)
		{
			var Result = CommandUtils.CombinePaths(Environment.SystemDirectory, ExeName);
			if (!CommandUtils.FileExists(Result))
			{
				// Use Regex.Replace so we can do a case-insensitive replacement of System32
				var SysNativeDirectory = Regex.Replace(Environment.SystemDirectory, "System32", "Sysnative", RegexOptions.IgnoreCase);
				var SysNativeExe = CommandUtils.CombinePaths(SysNativeDirectory, ExeName);
				if (CommandUtils.FileExists(SysNativeExe))
				{
					Result = SysNativeExe;
				}
			}
			return Result;
		}

		void LogSettings()
		{
			Log.TraceVerbose("Command Environment settings:");
			Log.TraceVerbose("CmdExe={0}", CmdExe);
			Log.TraceVerbose("EngineSavedFolder={0}", EngineSavedFolder);
			Log.TraceVerbose("HasCapabilityToCompile={0}", HasCapabilityToCompile);
			Log.TraceVerbose("LocalRoot={0}", LocalRoot);
			Log.TraceVerbose("LogFolder={0}", LogFolder);
			Log.TraceVerbose("MountExe={0}", MountExe);
			Log.TraceVerbose("MsBuildExe={0}", MsBuildExe);
			Log.TraceVerbose("RobocopyExe={0}", RobocopyExe);
			Log.TraceVerbose("TimestampAsString={0}", TimestampAsString);
			Log.TraceVerbose("UATExe={0}", UATExe);			
		}

		#region Compiler Setup

		/// <summary>
		/// Initializes build environemnt: finds the path to msbuild.exe
		/// </summary>
		void SetupBuildEnvironment()
		{
			// Assume we have the capability co compile.
			HasCapabilityToCompile = true;

			if (HasCapabilityToCompile)
			{
				try
				{
					MsBuildExe = HostPlatform.Current.GetMsBuildExe();
				}
				catch (Exception Ex)
				{
					Log.WriteLine(LogEventType.Warning, Ex.Message);
					Log.WriteLine(LogEventType.Warning, "Assuming no compilation capability.");
					HasCapabilityToCompile = false;
					MsBuildExe = "";
				}
			}

			Log.TraceVerbose("CompilationEvironment.HasCapabilityToCompile={0}", HasCapabilityToCompile);
			Log.TraceVerbose("CompilationEvironment.MsBuildExe={0}", MsBuildExe);
		}

		#endregion
	}

}
