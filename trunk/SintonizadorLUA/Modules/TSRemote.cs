//===========================================================================================
// Author    : James Mimeault
// Date      : 07/23/2004
// Comments  : This is a preliminary work in progress. I've put quite a few hours of research
//           : into this, so if you use it, please, give credit where it is due. Thanks!
//===========================================================================================
// Revisions |
//===========================================================================================
// 07/23/2004: JM: Initial (1.0.0.0)
// 02/18/2005: JM: Changed the Open method to be static.
// 02/22/2005: JM: Changed the SwitchChannels methods to be static.
//           :
//===========================================================================================
// Issues    |
//===========================================================================================
// 07/23/2004: JM: Not much in the way of exception handling here, will be implementing it
//           : for the next revision
// 07/23/2004: JM: Using the code to Connect, then immediately calling another method will
//           : result in an error in the Client. To get around this, make the connection a
//           : separate call or make the code suspend for some time before making another
//           : call to the client.
// 07/23/2004: JM: Documentation on the member functions is lacking, but I was being lazy.
// 07/23/2004: JM: TSRemote.GetChannelInfo() methods with the ref TtsrPlayerInfo[] parameter
//           : requires an initialized array. 
//           :   You need only initialize the array as follows:
//           : =============================================================================
//           :      TSRemote.TtsrUserInfo [] pi = new TSRemote.TtsrUserInfo[0];
//===========================================================================================

using System;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace YourNameSpaceHere
{
	/// <summary>
	/// Provides a wrapper for the exported methods in TSRemote.dll.
	/// Also provides the Classes and Flag enumerations for various
	/// calls and returns from TSRemote.dll.
	/// </summary>
	internal class TSRemote
	{
		#region Public bitmasked types for use with TSRemote

		[Flags()]
			public enum Codecs
		{
			cmCelp51        = 1,
			cmCelp63        = 2,
			cmGSM148        = 4,
			cmGSM164        = 8,
			cmWindowsCELP52 = 16,
			SPEEX2150       = 32,  
			SPEEX3950       = 64, 
			SPEEX5950       = 128, 
			SPEEX8000       = 256, 
			SPEEX11000      = 512, 
			SPEEX15000      = 1024, 
			SPEEX18200      = 2048, 
			SPEEX24600      = 4096 
		}

		//PlayerChannelPrivileges
		[Flags()]
			public enum ChannelPrivileges
		{
			Admin        = 1,
			Operator     = 2,
			AutoOperator = 4,
			Voiced       = 8,
			AutoVoice    = 16
		}

		//UserPrivileges
		[Flags()]
			public enum UserPrivileges
		{
			SuperServerAdmin = 1,
			ServerAdmin      = 2,
			CanRegister      = 4,
			Registered       = 8,
			Unregistered     = 16
		}
		
		//player flags
		[Flags()]
			public enum PlayerFlags
		{
			ChannelCommander = 1,
			WantVoice        = 2,
			NoWhisper        = 4,
			Away             = 8,
			InputMuted       = 16,
			OutputMuted      = 32,
			Recording        = 64
		}

		//channel flags
		[Flags()]
			public enum ChannelFlags
		{
			Registered   = 1,
			Unregistered = 2,
			Moderated    = 4,
			Password     = 8,
			Hierarchical = 16,
			Default      = 32
		}

		//ServerType Flags
		[Flags()]
			public enum ServerType
		{
			stClan       = 1,
			stPublic     = 2,
			stFreeware   = 4,
			stCommercial = 8
		}

		#endregion

		#region Public Classes for use with TSRemote

		[ StructLayout( LayoutKind.Sequential, CharSet=CharSet.Ansi ) ]
			public class TtsrPlayerInfo
		{
			public int PlayerID = 0;
			public int ChannelID = 0;
			[ MarshalAs( UnmanagedType.ByValTStr, SizeConst=30 ) ]
			public string NickName = "";
			public int PlayerChannelPrivileges = 0;
			public int PlayerPrivileges = 0;
			public int PlayerFlags = 0;

			public TtsrPlayerInfo()
			{
				PlayerID                = 0;
				ChannelID               = 0;
				NickName                = new string ( '\0', 30 );
				PlayerChannelPrivileges = 0;
				PlayerPrivileges        = 0;
				PlayerFlags             = 0;
			}

			internal int ReadHeapValues( IntPtr ip )
			{
				if ( ip.ToInt32() != 0 )
				{
					int offset   = 0;
					int int_size = Marshal.SizeOf( typeof(System.Int32) );
					int str_size = 0;

					PlayerID = Marshal.ReadInt32( ip );

					offset += int_size;
					ChannelID = Marshal.ReadInt32( new IntPtr( ip.ToInt32() + offset ) );

					offset += int_size;
					str_size = NickName.Length;
					NickName = Marshal.PtrToStringAnsi( new IntPtr( ip.ToInt32() + offset ) );

					offset += str_size;
					PlayerChannelPrivileges = Marshal.ReadInt32( new IntPtr( ip.ToInt32() + offset ) );

					offset += int_size;
					PlayerPrivileges = Marshal.ReadInt32( new IntPtr( ip.ToInt32() + offset ) );

					offset += int_size;
					PlayerFlags = Marshal.ReadInt32( new IntPtr( ip.ToInt32() + offset ) );
		
					return 0;
				}
				return -1;
			}
		}

		[ StructLayout( LayoutKind.Sequential, CharSet=CharSet.Ansi ) ]
			public class TtsrChannelInfo
		{
			public int ChannelID;
			public int ChannelParentID;
			public int PlayerCountInChannel;
			public int ChannelFlags;
			public int Codec;
			[ MarshalAs( UnmanagedType.ByValTStr, SizeConst=30 ) ]
			public string Name;

			public TtsrChannelInfo()
			{
				ChannelID            = 0;
				ChannelParentID      = 0;
				PlayerCountInChannel = 0;
				ChannelFlags         = 0;
				Codec         = 0;
				Name                 = new string( '\0', 30 );
			}

			internal int ReadHeapValues( IntPtr ip )
			{
				if ( ip.ToInt32() != 0 )
				{
					int offset   = 0;
					int int_size = Marshal.SizeOf( typeof(System.Int32) );

					ChannelID = Marshal.ReadInt32( ip );
					offset += int_size;

					ChannelParentID = Marshal.ReadInt32( new IntPtr( ip.ToInt32() + offset ) );
					offset += int_size;

					PlayerCountInChannel = Marshal.ReadInt32( new IntPtr( ip.ToInt32() + offset ) );
					offset += int_size;

					ChannelFlags = Marshal.ReadInt32( new IntPtr( ip.ToInt32() + offset ) );
					offset += int_size;

					Codec = Marshal.ReadInt32( new IntPtr( ip.ToInt32() + offset ) );
					offset += int_size;

					Name = Marshal.PtrToStringAnsi( new IntPtr( ip.ToInt32() + offset ) );
		
					return 0;
				}
				return -1;
			}
		}

		[ StructLayout( LayoutKind.Sequential, CharSet=CharSet.Ansi ) ]
			public class TtsrVersion
		{
			public int Major   = 0;
			public int Minor   = 0;
			public int Release = 0;
			public int Build   = 0;
		}

		[ StructLayout( LayoutKind.Sequential, CharSet=CharSet.Ansi ) ]
			public class TtsrServerInfo
		{
			[ MarshalAs( UnmanagedType.ByValTStr, SizeConst=30 ) ]
			public string ServerName;
			[ MarshalAs( UnmanagedType.ByValTStr, SizeConst=256 ) ]
			public string WelcomeMessage;
			public int ServerVMajor;
			public int ServerVMinor;
			public int ServerVRelease;
			public int ServerVBuild;
			[ MarshalAs( UnmanagedType.ByValTStr, SizeConst=30 ) ]
			public string ServerPlatform;
			[ MarshalAs( UnmanagedType.ByValTStr, SizeConst=30 ) ]
			public string ServerIp;
			[ MarshalAs( UnmanagedType.ByValTStr, SizeConst=100 ) ]
			public string ServerHost;
			public int ServerType;
			public int ServerMaxUsers;
			public int SupportedCodecs;
			public int ChannelCount;
			public int PlayerCount;

			public TtsrServerInfo()
			{
				ServerName      = new string( '\0', 30 );
				WelcomeMessage  = new string( '\0', 256 );
				ServerVMajor    = 0;
				ServerVMinor    = 0;
				ServerVRelease  = 0;
				ServerVBuild    = 0;
				ServerPlatform  = new string( '\0', 30 );
				ServerIp        = new string( '\0', 30 );
				ServerHost      = new string( '\0', 100 );
				ServerType      = 0;
				ServerMaxUsers  = 0;
				SupportedCodecs = 0;
				ChannelCount    = 0;
				PlayerCount     = 0;
			}

			internal int ReadHeapValues( IntPtr ip )
			{
				if ( ip.ToInt32() != 0 )
				{
					int offset   = 0;
					int int_size = Marshal.SizeOf( offset );
					int str_size = 0;

					str_size = ServerName.Length;
					ServerName = Marshal.PtrToStringAnsi( ip );

					offset += str_size;
					str_size = WelcomeMessage.Length;
					WelcomeMessage = Marshal.PtrToStringAnsi( new IntPtr( ip.ToInt32() + offset ) );

					offset += str_size;
					ServerVMajor = Marshal.ReadInt32  ( new IntPtr( ip.ToInt32() + offset ) );

					offset += int_size;
					ServerVMinor = Marshal.ReadInt32  ( new IntPtr( ip.ToInt32() + offset ) );

					offset += int_size;
					ServerVRelease = Marshal.ReadInt32( new IntPtr( ip.ToInt32() + offset ) );

					offset += int_size;
					ServerVBuild = Marshal.ReadInt32  ( new IntPtr( ip.ToInt32() + offset ) );

					offset += int_size;
					str_size = ServerPlatform.Length;
					ServerPlatform = Marshal.PtrToStringAnsi( new IntPtr( ip.ToInt32() + offset ) );

					offset += str_size;
					str_size = ServerIp.Length;
					ServerIp = Marshal.PtrToStringAnsi( new IntPtr( ip.ToInt32() + offset ) );

					offset += str_size;
					str_size = ServerHost.Length;
					ServerHost = Marshal.PtrToStringAnsi( new IntPtr( ip.ToInt32() + offset ) );

					offset += str_size;
					ServerType = Marshal.ReadInt32( new IntPtr( ip.ToInt32() + offset ) );

					offset += int_size;
					ServerMaxUsers = Marshal.ReadInt32( new IntPtr( ip.ToInt32() + offset ) );

					offset += int_size;
					SupportedCodecs = Marshal.ReadInt32( new IntPtr( ip.ToInt32() + offset ) );

					offset += int_size;
					ChannelCount = Marshal.ReadInt32( new IntPtr( ip.ToInt32() + offset ) );

					offset += int_size;
					PlayerCount = Marshal.ReadInt32( new IntPtr( ip.ToInt32() + offset ) );
		
					return 0;
				}
				return -1;
			}
		}

		[ StructLayout( LayoutKind.Sequential, CharSet=CharSet.Ansi ) ]
			public class TtsrUserInfo
		{
			public TtsrPlayerInfo  Player;
			public TtsrChannelInfo Channel;
			public TtsrChannelInfo ParentChannel;

			public TtsrUserInfo()
			{
				Player        = new TtsrPlayerInfo();
				Channel       = new TtsrChannelInfo();
				ParentChannel = new TtsrChannelInfo();
			}

			internal int ReadHeapValues( IntPtr ip )
			{
				if ( ip.ToInt32() != 0 )
				{
					int offset   = 0;
					int int_size = Marshal.SizeOf( typeof(System.Int32) );

					// Getting PlayerInfo
					Player.ReadHeapValues( ip );
					offset = Marshal.SizeOf( typeof( TSRemote.TtsrPlayerInfo ) );

					Channel.ReadHeapValues( new IntPtr( ip.ToInt32() + offset - 2) );
					offset += Marshal.SizeOf( typeof( TSRemote.TtsrChannelInfo ) );

					ParentChannel.ReadHeapValues( new IntPtr( ip.ToInt32() + offset - 4 ) );

					return 0;
				}
				return -1;
			}
		}


		#endregion

		#region TSRemote Imports

		/// <summary>
		/// Get the full error message that was send with the last error
		/// </summary>
		/// <param name="pchBuffer">A pointer to a null terminated string where the error message will be copied to.</param>
		/// <param name="BufferLength">The size of pchBuffer</param>
		//		[ DllImport( "TSRemote.dll" ) ]
		//		private static extern void tsrGetLastError( [In, Out] string pchBuffer, [In, Out] int BufferLength );
		[ DllImport( "TSRemote.dll" ) ]
		private static extern void tsrGetLastError( IntPtr pchBuffer, int BufferLength );

		/// <summary>
		/// Connect the ts to a new server as described in the URL. Will disconnect
		/// if the client is currently connected. The Url is the same format as the
		/// normal starup link ("teamspeak://voice.teamspeak.org:9500" etc)
		/// </summary>
		/// <param name="URL">A pointer to a null terminated string containing the url for the server.</param>
		/// <returns>Result: 0 = OK, else the error number</returns>
		[ DllImport( "TSRemote.dll" ) ]
		private static extern int tsrConnect( string URL );

		/// <summary>
		/// Disconnects the client from the current server
		/// </summary>
		/// <returns>Result: 0 = OK, else the error number</returns>
		[ DllImport( "TSRemote.dll" ) ]
		private static extern int tsrDisconnect();

		/// <summary>
		/// Disconnect, Close and terminate the client
		/// </summary>
		/// <returns>Result: 0 = OK, else the error number</returns>
		[ DllImport( "TSRemote.dll" ) ]
		private static extern int tsrQuit();

		/// <summary>
		/// Switch to the channel with the name "Channelname"
		/// Note that tsrSwitchChannelID is preferred.
		/// </summary>
		/// <param name="ChannelName">Name of the channel to switch to</param>
		/// <param name="ChannelPassword">Password for the channel. May be null</param>
		/// <returns>Result: 0 = OK, else the error number</returns>
		[ DllImport( "TSRemote.dll" ) ]
		private static extern int tsrSwitchChannelName( string ChannelName, string ChannelPassword );

		/// <summary>
		/// Switch to the channel with the id "ChannelID"
		/// </summary>
		/// <param name="ChannelID">ID of the channel to switch to</param>
		/// <param name="ChannelPassword">Password for the channel. May be null</param>
		/// <returns>Result: 0 = OK, else the error number</returns>
		[ DllImport( "TSRemote.dll" ) ]
		private static extern int tsrSwitchChannelID( int ChannelID, string ChannelPassword );

		/// <summary>
		/// Get the version of the TS Client
		/// </summary>
		/// <param name="tsrVersion">Pointer to a TtsrVersion record</param>
		/// <returns>
		/// Result: 0 = OK, else the error number
		/// If result = 0 then tsrVersion is filled with the client version
		/// </returns>
		[ DllImport( "TSRemote.dll" ) ]
		private static extern int tsrGetVersion( [In, Out] TtsrVersion tsrVersion );

		/// <summary>
		/// Get the Info on the server (name, channelcount, playercount etc etc)
		/// </summary>
		/// <param name="tsrServerInfo">Pointer to a TtsrServerInfo record</param>
		/// <returns>
		/// Result: 0 = OK, else the error number
		/// If result = 0 then tsrServerInfo is filled with the server info
		/// </returns>
		[ DllImport( "TSRemote.dll" ) ]
		private static extern int tsrGetServerInfo( IntPtr tsrServerInfo );

		/// <summary>
		/// Get the Info on the user (name, channel, flags etc etc)
		/// </summary>
		/// <param name="tsrUserInfo">Pointer to a TtsrUserInfo record</param>
		/// <returns>
		/// Result: 0 = OK, else the error number
		/// If result = 0 then tsrUserInfo is filled with the user info
		/// </returns>
		[ DllImport( "TSRemote.dll" ) ]
		private static extern int tsrGetUserInfo( IntPtr tsrUserInfo );

		/// <summary>
		/// Get the Info on the channel specified by ChannelID and optionally also get the users from that channel
		/// </summary>
		/// <param name="ChannelID">The ID of the channel you want the info from</param>
		/// <param name="tsrChannelInfo">pointer to a TtsrChannelInfo record</param>
		/// <param name="tsrPlayerInfo"> This is the pointer to an array of TtsrPlayerInfo records. If it is NIL, no player records will be retrieved</param>
		/// <param name="PlayerRecords">Pointer to an integer. It must contain how many records tsrPlayerInfo has room for. (records, not bytes)</param>
		/// <returns>
		/// Result: 0 = OK, else the error number
		/// If result = 0 then tsrChannelInfo is filled with the channel info.
		/// If tsrPlayerInfo was not NIL then the player records are
		/// filled. PlayerRecords indicates how many records were filled
		/// </returns>
		[ DllImport( "TSRemote.dll" ) ]
		private static extern int tsrGetChannelInfoByID( int ChannelID, 
			IntPtr tsrChannelInfo, 
			IntPtr tsrPlayerInfo, 
			out int PlayerRecords );

		/// <summary>
		/// Get the Info on the channel specified by ChannelName and optionally also get the users from that channel
		/// </summary>
		/// <param name="ChannelName">The Name of the channel you want the info from</param>
		/// <param name="tsrChannelInfo">pointer to a TtsrChannelInfo record</param>
		/// <param name="tsrPlayerInfo"> This is the pointer to an array of TtsrPlayerInfo records. If it is NIL, no player records will be retrieved</param>
		/// <param name="PlayerRecords">Pointer to an integer. It must contain how many records tsrPlayerInfo has room for. (records, not bytes)</param>
		/// <returns>
		/// Result: 0 = OK, else the error number
		/// If result = 0 then tsrChannelInfo is filled with the channel info.
		/// If tsrPlayerInfo was not NIL then the player records are
		/// filled. PlayerRecords indicates how many records were filled
		/// </returns>
		[ DllImport( "TSRemote.dll" ) ]
		private static extern int tsrGetChannelInfoByName( string ChannelName, 
			IntPtr tsrChannelInfo, 
			IntPtr tsrPlayerInfo, 
			out int PlayerRecords );

		/// <summary>
		/// Get the Info on the player specified by PlayerID
		/// </summary>
		/// <param name="PlayerID">The ID of the player you want the info from</param>
		/// <param name="tsrPlayerInfo">This is the pointer to a TtsrPlayerInfo record</param>
		/// <returns>
		/// Result: 0 = OK, else the error number
		/// If result = 0 then tsrPlayerInfo is filled with the player info.
		/// </returns>
		[ DllImport( "TSRemote.dll" ) ]
		private static extern int tsrGetPlayerInfoByID( int PlayerID, 
			IntPtr tsrPlayerInfo );

		/// <summary>
		/// Get the Info on the player specified by PlayerName
		/// </summary>
		/// <param name="PlayerID">The Name of the player you want the info from</param>
		/// <param name="tsrPlayerInfo">This is the pointer to a TtsrPlayerInfo record</param>
		/// <returns>
		/// Result: 0 = OK, else the error number
		/// If result = 0 then tsrPlayerInfo is filled with the player info.
		/// </returns>
		[ DllImport( "TSRemote.dll" ) ]
		private static extern int tsrGetPlayerInfoByName( string PlayerName, 
			IntPtr tsrPlayerInfo );

		/// <summary>
		/// Get a list of the channels on the server
		/// </summary>
		/// <param name="tsrChannels">A pointer to an array of TtsrChannelInfo records</param>
		/// <param name="ChannelRecords">pointer to an integer which specifies how many TtsrChannelInfo records tsrChannels can hold.</param>
		/// <returns>
		/// Result: 0 = OK, else the error number
		/// If result = 0 then tsrChannels is filled with the channel info.
		/// ChannelRecords will have the number or records that were filled.
		/// </returns>
		[ DllImport( "TSRemote.dll" ) ]
		private static extern int tsrGetChannels( IntPtr tsrChannels, 
			out int ChannelRecords );

		/// <summary>
		/// Get a list of the Players on the server
		/// </summary>
		/// <param name="tsrPlayers">A pointer to an array of TtsrPlayerInfo records</param>
		/// <param name="PlayerRecords">pointer to an integer which specifies how many TtsrPlayerInfo records tsrPlayers can hold</param>
		/// <returns>
		/// Result: 0 = OK, else the error number
		/// PlayerRecords will have the number or records that were filled
		/// </returns>
		[ DllImport( "TSRemote.dll" ) ]
		private static extern int tsrGetPlayers( IntPtr tsrPlayers, 
			out int PlayerRecords );

		/// <summary>
		/// 
		/// </summary>
		/// <param name="tsrPlayerIDs">A pointer to an array of integers</param>
		/// <param name="PlayerRecords">pointer to an integer which specifies how many TtsrPlayerInfo records tsrPlayers can hold</param>
		/// <returns>
		/// Result: 0 = OK, else the error number
		/// PlayerRecords will have the number or records that were filled
		/// </returns>
		[DllImport( "TSRemote.dll" )]
		private static extern int tsrGetSpeakers( int [] tsrPlayerIDs, out int PlayerRecords );

		/// <summary>
		/// Set player flags of the user
		/// </summary>
		/// <param name="tsrPlayerFlags">An integer which has the bitmask for all the flags. All flags are set to this bitmask.</param>
		/// <returns>Result: 0 = OK, else the error number</returns>
		[ DllImport( "TSRemote.dll" ) ]
		private static extern int tsrSetPlayerFlags( int tsrPlayerFlags );

		/// <summary>
		/// Set the reason you want voice on a channel
		/// </summary>
		/// <param name="tsrReason">The reseason for voice</param>
		/// <returns>Result: 0 = OK, else the error number</returns>
		[ DllImport( "TSRemote.dll" ) ]
		private static extern int tsrSetWantVoiceReason( string tsrReason );

		/// <summary>
		/// Grant or revoke Operator status
		/// </summary>
		/// <param name="PlayerID">ID of the player to set the operator status for</param>
		/// <param name="GrantRevoke">Set to TSRemoteWrap.GRANT to grant or TSRemoteWrap.REVOKE to revoke the privilege</param>
		/// <returns>Result: 0 = OK, else the error number</returns>
		[ DllImport( "TSRemote.dll" ) ]
		private static extern int tsrSetOperator( int PlayerID, int GrantRevoke );

		/// <summary>
		/// Grant or revoke Voice status
		/// </summary>
		/// <param name="PlayerID">ID of the player to set the Voice status for</param>
		/// <param name="GrantRevoke">Set to TSRemoteWrap.GRANT to grant or TSRemoteWrap.REVOKE to revoke the privilege</param>
		/// <returns>Result: 0 = OK, else the error number</returns>
		[ DllImport( "TSRemote.dll" ) ]
		private static extern int tsrSetVoice( int PlayerID, int GrantRevoke );

		/// <summary>
		/// Kick a player from the server
		/// </summary>
		/// <param name="PlayerID">ID of the player to set the Voice status for</param>
		/// <param name="Reason">The reason why he was kicked</param>
		/// <returns>Result: 0 = OK, else the error number</returns>
		[ DllImport( "TSRemote.dll" ) ]
		private static extern int tsrKickPlayerFromServer( int PlayerID, string Reason );
		
		/// <summary>
		/// Kick a player from the Channel
		/// </summary>
		/// <param name="PlayerID">ID of the player to set the Voice status for</param>
		/// <param name="Reason">The reason why he was kicked</param>
		/// <returns>Result: 0 = OK, else the error number</returns>
		[ DllImport( "TSRemote.dll" ) ]
		private static extern int tsrKickPlayerFromChannel( int PlayerID, string Reason );

		/// <summary>
		/// Send a text message to a channel
		/// </summary>
		/// <param name="ChannelID">The ID of the channel you want to send the txt message to</param>
		/// <param name="Message">The message you want to send</param>
		/// <returns>Result: 0 = OK, else the error number</returns>
		[ DllImport( "TSRemote.dll" ) ]
		private static extern int tsrSendTextMessageToChannel( int ChannelID, string Message );

		/// <summary>
		/// Send a text message to everyone
		/// </summary>
		/// <param name="Message">The message you want to send</param>
		/// <returns>Result: 0 = OK, else the error number</returns>
		[ DllImport( "TSRemote.dll" ) ]
		private static extern int tsrSendTextMessage( string Message );

		#endregion

		#region Public (static) TSRemote interface

		public TSRemote()
		{
			//
			// TODO: Add constructor logic here
			//
		}

		// will attempt to open the TeamSpeak Client for you
		public static int Open()
		{
			Process proc = null;
			Process [] procs = null;

			try
			{
				procs = Process.GetProcessesByName( "TeamSpeak" );

				if ( procs.Length == 0 )
				{
					Microsoft.Win32.RegistryKey RegKey = Microsoft.Win32.Registry.ClassesRoot;
					Microsoft.Win32.RegistryKey RegValue = RegKey.OpenSubKey( "teamspeak\\Shell\\Open\\command" );

					string Value = (string)RegValue.GetValue( "" );

					Value = Value.Replace( "\"%1\"", "" );
					Value = Value.Trim();

					proc = new Process();
					proc.StartInfo.FileName = Value;
					proc.StartInfo.Verb = proc.StartInfo.Verbs[0];
					proc.Start();
				}
				else
				{
					proc = procs[0];
				}
				if ( proc == null )
				{
					throw new Exception( "Unable to find or create process 'TeamSpeak'" );
				}
			}
			catch ( Exception e )
			{
				throw ( e );
			}
			return ( proc == null ? -1 : 0 );
		}

		public static string GetLastError()
		{
			string error_str       = new string( '\0', 256 );
			int size               = error_str.Length;
			System.IntPtr p_strErr = Marshal.AllocHGlobal(size);

			tsrGetLastError( p_strErr, size );

			error_str = Marshal.PtrToStringAnsi( p_strErr );
			Marshal.FreeHGlobal( p_strErr );

			System.GC.Collect();

			return error_str;
		}

		/// <summary>
		/// Connects to the TeamSpeak server specified in URL
		/// </summary>
		/// <param name="URL">
		/// URL Parts:
		/// 1) teamspeak://
		/// 2) www.serveraddress.com OR IP address
		/// 3) : (a colon)
		/// 4) port number
		/// 5) ?nickname=and the nickname (if unregistered)
		/// 6) ?loginname=and the login name (if registered)
		/// 7) ?channelname=and the channel name
		/// 7) ?password=and the password for the server/channel
		/// 
		/// Note that if you do not use a parameter, you should leave it off, as in the following:
		/// teamspeak://192.168.1.150:8767?nickname=guest?loginname=admin?password=admin
		/// </param>
		/// <returns>0 if successful otherwise non-zero, use GetLastError()</returns>
		public static int Connect( string URL )
		{
			return tsrConnect( URL );
		}

		public static int Disconnect()
		{
			return tsrDisconnect();
		}

		public static int Quit()
		{
			return tsrQuit();
		}

		public static int SwitchChannels( string channel_name, string password )
		{
			int nRetVal = 0;

			nRetVal = tsrSwitchChannelName( channel_name, password );

			return nRetVal;
		}

		public static int SwitchChannels( int channel_id, string password )
		{
			int nRetVal = 0;

			nRetVal = tsrSwitchChannelID( channel_id, password );

			return nRetVal;
		}

		public static TtsrVersion ClientVersion
		{
			get
			{
				int nRetVal    = 0;
				TtsrVersion vi = new TtsrVersion();

				nRetVal = tsrGetVersion( vi );

				return vi;
			}
		}

		public static TtsrServerInfo ServerInfo
		{
			get
			{
				int nRetVal       = 0;
				int size          = Marshal.SizeOf( typeof( TtsrServerInfo ) );

				TtsrServerInfo si = new TtsrServerInfo();

				System.IntPtr p_current = Marshal.AllocHGlobal(size);

				nRetVal = tsrGetServerInfo( p_current );

				if ( nRetVal == 0 )
				{
					// this is where you'll create a new object and copy the data over instead of
					// using this stupid array.
					si.ReadHeapValues( p_current );

					Marshal.DestroyStructure( p_current, typeof(TtsrServerInfo) );
				}

				// Free the memory that was allocated on the heap, otherwise
				// you will create a memory leak. 
				Marshal.FreeHGlobal(p_current);

				return si;
			}
		}

		public static TtsrUserInfo UserInfo
		{
			get
			{
				int     nRetVal    = 0;
				TtsrUserInfo ui    = new TtsrUserInfo();
				int        size    = Marshal.SizeOf( typeof( TtsrUserInfo ) );

				System.IntPtr p_current = Marshal.AllocHGlobal( size );

				nRetVal = tsrGetUserInfo( p_current );

				if ( nRetVal == 0 )
				{
					nRetVal = ui.ReadHeapValues( p_current );
					Marshal.DestroyStructure( p_current, typeof( TtsrUserInfo ) );
				}

				// Free the memory that was allocated on the heap, otherwise
				// you will create a memory leak. 
				Marshal.FreeHGlobal(p_current);

				return ui;
			}
		}

		// gets Channel Info and Players by ID
		public static TtsrChannelInfo GetChannelInfo( int channel_id, ref TtsrPlayerInfo [] arr_pi )
		{
			const int MAX_PLAYERS = 200;
			int        nRetVal    = 0;
			int        records    = MAX_PLAYERS;
			bool bGetPlayers      = false;
			TtsrChannelInfo ci    = new TtsrChannelInfo();
			TtsrPlayerInfo [] pi  = new TtsrPlayerInfo[MAX_PLAYERS];

			int        ci_size    = Marshal.SizeOf( typeof( TtsrChannelInfo ) );
			int        pi_size    = Marshal.SizeOf( typeof( TtsrPlayerInfo  ) ) * MAX_PLAYERS;

			System.IntPtr p_channel = Marshal.AllocHGlobal( ci_size );
			System.IntPtr p_player  = Marshal.AllocHGlobal( pi_size );
			System.IntPtr p_current = (IntPtr)0;

			nRetVal = tsrGetChannelInfoByID( channel_id, p_channel, p_player, out records );

			// figure out if we are to get the player list too.
			if ( arr_pi != null )
			{
				bGetPlayers = true;
			}
			arr_pi = null;

			// now process the items
			if ( nRetVal == 0 )
			{
				nRetVal = ci.ReadHeapValues( p_channel );
				Marshal.DestroyStructure( p_channel, typeof( TtsrChannelInfo ) );

				if ( nRetVal == 0 && bGetPlayers == true )
				{
					// build the new player array...
					arr_pi = new TtsrPlayerInfo [ records ];

					// keep track of our player pointer
					p_current = p_player;

					for( int i = 0; i < records; i++ )
					{
						arr_pi[ i ] = new TtsrPlayerInfo();

						Marshal.PtrToStructure  ( p_current, arr_pi[ i ]);
						Marshal.DestroyStructure( p_current, typeof(TtsrPlayerInfo) );

						p_current = (IntPtr)((int)p_current + Marshal.SizeOf( typeof( TtsrPlayerInfo ) ) - 2 );
					}
				}
			}

			// Free the memory that was allocated on the heap, otherwise
			// you will create a memory leak. 
			Marshal.FreeHGlobal(p_channel);
			Marshal.FreeHGlobal(p_player);

			return ci;
		}
		
		// gets Channel Info by ID
		public static TtsrChannelInfo GetChannelInfo( int channel_id )
		{
			int        nRetVal    = 0;
			int        records    = 0;
			TtsrChannelInfo ci    = new TtsrChannelInfo();

			int        ci_size    = Marshal.SizeOf( typeof( TtsrChannelInfo ) );

			System.IntPtr p_channel = Marshal.AllocHGlobal( ci_size );

			nRetVal = tsrGetChannelInfoByID( channel_id, p_channel, (IntPtr)0, out records );

			// now process the items
			if ( nRetVal == 0 )
			{
				nRetVal = ci.ReadHeapValues( p_channel );
				Marshal.DestroyStructure( p_channel, typeof( TtsrChannelInfo ) );
			}

			// Free the memory that was allocated on the heap, otherwise
			// you will create a memory leak. 
			Marshal.FreeHGlobal(p_channel);

			return ci;
		}
		
		// gets Channel Info and Players by Name
		public static TtsrChannelInfo GetChannelInfo( string channel_name, ref TtsrPlayerInfo [] arr_pi )
		{
			const int MAX_PLAYERS = 200;
			int        nRetVal    = 0;
			int        records    = MAX_PLAYERS;
			bool bGetPlayers      = false;
			TtsrChannelInfo ci    = new TtsrChannelInfo();
			TtsrPlayerInfo [] pi  = new TtsrPlayerInfo[MAX_PLAYERS];

			int        ci_size    = Marshal.SizeOf( typeof( TtsrChannelInfo ) );
			int        pi_size    = Marshal.SizeOf( typeof( TtsrPlayerInfo  ) ) * MAX_PLAYERS;

			System.IntPtr p_channel = Marshal.AllocHGlobal( ci_size );
			System.IntPtr p_player  = Marshal.AllocHGlobal( pi_size );
			System.IntPtr p_current = (IntPtr)0;

			nRetVal = tsrGetChannelInfoByName( channel_name, p_channel, p_player, out records );

			// figure out if we are to get the player list too.
			if ( arr_pi != null )
			{
				bGetPlayers = true;
			}
			arr_pi = null;

			// now process the items
			if ( nRetVal == 0 )
			{
				nRetVal = ci.ReadHeapValues( p_channel );
				Marshal.DestroyStructure( p_channel, typeof( TtsrChannelInfo ) );

				if ( nRetVal == 0 && bGetPlayers == true )
				{
					// build the new player array...
					arr_pi = new TtsrPlayerInfo [ records ];

					// keep track of our player pointer
					p_current = p_player;

					for( int i = 0; i < records; i++ )
					{
						arr_pi[ i ] = new TtsrPlayerInfo();

						Marshal.PtrToStructure  ( p_current, arr_pi[ i ]);
						Marshal.DestroyStructure( p_current, typeof(TtsrPlayerInfo) );

						p_current = (IntPtr)((int)p_current + Marshal.SizeOf( typeof( TtsrPlayerInfo ) ) - 2 );
					}
				}
			}

			// Free the memory that was allocated on the heap, otherwise
			// you will create a memory leak. 
			Marshal.FreeHGlobal(p_channel);
			Marshal.FreeHGlobal(p_player);

			return ci;
		}
		
		// gets Channel Info by Name
		public static TtsrChannelInfo GetChannelInfo( string channel_name )
		{
			int        nRetVal    = 0;
			int        records    = 0;
			TtsrChannelInfo ci    = new TtsrChannelInfo();

			int        ci_size    = Marshal.SizeOf( typeof( TtsrChannelInfo ) );

			System.IntPtr p_channel = Marshal.AllocHGlobal( ci_size );

			nRetVal = tsrGetChannelInfoByName( channel_name, p_channel, (IntPtr)0, out records );

			// now process the items
			if ( nRetVal == 0 )
			{
				nRetVal = ci.ReadHeapValues( p_channel );
				Marshal.DestroyStructure( p_channel, typeof( TtsrChannelInfo ) );
			}

			// Free the memory that was allocated on the heap, otherwise
			// you will create a memory leak. 
			Marshal.FreeHGlobal(p_channel);

			return ci;
		}
		
		// gets Player Info by ID
		public static TtsrPlayerInfo GetPlayerInfo( int player_id )
		{
			int        nRetVal = 0;
			TtsrPlayerInfo pi  = new TtsrPlayerInfo();
			int        size    = Marshal.SizeOf( typeof( TtsrPlayerInfo ) );

			System.IntPtr p_player = Marshal.AllocHGlobal(size);

			nRetVal = tsrGetPlayerInfoByID( player_id, p_player );

			if ( nRetVal == 0 )
			{
				nRetVal = pi.ReadHeapValues( p_player );
			}

			Marshal.DestroyStructure( p_player, typeof( TtsrPlayerInfo ) );

			// Free the memory that was allocated on the heap, otherwise
			// you will create a memory leak. 
			Marshal.FreeHGlobal(p_player);

			return pi;
		}

		// gets Player Info by Name
		public static TtsrPlayerInfo GetPlayerInfo( string player_name )
		{
			int        nRetVal = 0;
			TtsrPlayerInfo pi  = new TtsrPlayerInfo();
			int        size    = Marshal.SizeOf( typeof( TtsrPlayerInfo ) );

			System.IntPtr p_player = Marshal.AllocHGlobal(size);

			nRetVal = tsrGetPlayerInfoByName( player_name, p_player );

			if ( nRetVal == 0 )
			{
				nRetVal = pi.ReadHeapValues( p_player );
			}

			Marshal.DestroyStructure( p_player, typeof( TtsrPlayerInfo ) );

			// Free the memory that was allocated on the heap, otherwise
			// you will create a memory leak. 
			Marshal.FreeHGlobal(p_player);

			return pi;
		}

		public static TtsrChannelInfo [] Channels
		{
			get
			{
				int size = 10;
				int retval = 0;

				IntPtr p_array;
				IntPtr p_current;

				TtsrChannelInfo tci = new TtsrChannelInfo();

				p_array = Marshal.AllocHGlobal( Marshal.SizeOf( typeof(TtsrChannelInfo) ) * size );

				retval = tsrGetChannels( p_array, out size );

				TtsrChannelInfo[] channels = new TtsrChannelInfo[size];

				p_current = p_array;

				for( int i = 0; i < size; i++ )
				{
					channels[ i ] = new TtsrChannelInfo();

					// this is where you'll create a new object and copy the data over instead of
					// using this stupid array.
					Marshal.PtrToStructure  ( p_current, channels[ i ]);
					Marshal.DestroyStructure( p_current, typeof(TtsrChannelInfo) );

					p_current = (IntPtr)((int)p_current + Marshal.SizeOf( typeof(TtsrChannelInfo) ) - 2 );
				}
				Marshal.FreeHGlobal( p_array );

				System.GC.Collect();

				return channels;
			}
		}

		public static TtsrPlayerInfo [] Players
		{
			get
			{
				const int MAX_PLAYERS = 200;

				int    records = MAX_PLAYERS;

				int nRetVal       = 0;
				TtsrPlayerInfo [] arr_players;

				int size = Marshal.SizeOf( typeof( TtsrPlayerInfo ) ) * MAX_PLAYERS;

				System.IntPtr p_player = Marshal.AllocHGlobal(size);
				System.IntPtr p_current;

				nRetVal = tsrGetPlayers( p_player, out records );

				if ( nRetVal == 0 )
				{
					// keep track of our player pointer
					p_current = p_player;

					arr_players = new TtsrPlayerInfo[records];

					for ( int i = 0; i < records; i++ )
					{
						arr_players[i] = new TtsrPlayerInfo();

						nRetVal = arr_players[i].ReadHeapValues( p_current );

						Marshal.DestroyStructure( p_current, typeof( TtsrPlayerInfo ) );

						p_current = (IntPtr)((int)p_current + Marshal.SizeOf( typeof( TtsrPlayerInfo ) ) - 2 );
					}
				}
				else
				{
					arr_players = new TtsrPlayerInfo[0];
				}


				// Free the memory that was allocated on the heap, otherwise
				// you will create a memory leak. 
				Marshal.FreeHGlobal(p_player);

				return arr_players;
			}
		}

		public static int [] SpeakerIds
		{
			get
			{
				const int MAX_SPEAKERS = 200;

				int [] speakers   = new int [MAX_SPEAKERS];
				int [] arr_return = null;
				int records       = MAX_SPEAKERS;
				int nRetVal       = 0;

				tsrGetSpeakers( speakers, out records );
			
				if ( nRetVal == 0 )
				{
					arr_return = new int [records];
					Array.Copy( speakers, arr_return, records );
				}

				return arr_return;
			}
		}

		public static int SetPlayerFlags( int flags )
		{
			int nRetVal = 0;

			nRetVal = tsrSetPlayerFlags( flags );

			return nRetVal;
		}

		public static int SetWantVoiceReason( string reason )
		{
			int nRetVal = 0;

			nRetVal = tsrSetWantVoiceReason( reason );

			return nRetVal;
		}

		public static int SetOperator( int player_id, bool revoke )
		{
			int nRetVal = 0;
			int nGrantRevoke = 0 + (revoke == true ? 0 : 1);

			nRetVal = tsrSetOperator( player_id, nGrantRevoke );

			return nRetVal;
		}

		public static int SetVoice( int player_id, bool revoke )
		{
			int nRetVal = 0;
			int nGrantRevoke = 0 + (revoke == true ? 0 : 1);

			nRetVal = tsrSetVoice( player_id, nGrantRevoke );

			return nRetVal;
		}

		public static int KickPlayerFromServer( int player_id, string reason )
		{
			int nRetVal = 0;

			nRetVal = tsrKickPlayerFromServer( player_id, reason );

			return nRetVal;
		}

		public static int KickPlayerFromChannel( int player_id, string reason )
		{
			int nRetVal = 0;

			nRetVal = tsrKickPlayerFromChannel( player_id, reason );

			return nRetVal;
		}

		public static int SendTextToChannel( int channel_id, string message )
		{
			int nRetVal = 0;

			nRetVal = tsrSendTextMessageToChannel( channel_id, message );

			return nRetVal;
		}
		public static int SendText( string message )
		{
			int nRetVal = 0;

			nRetVal = tsrSendTextMessage( message );

			return nRetVal;
		}


		#endregion

		#region Public (static) BitMask to Array Converter

		public static string [] CodecArray( int Value )
		{
			string [] values;
			string value_list;
			Codecs codec_mask = (Codecs)Value;

			value_list = codec_mask.ToString("G");
			values = value_list.Split( ',' );

			for ( int i = 0; i < values.Length; i++ )
				values[i] = values[i].Trim();

			return values;
		}
		public static string [] ChannelPrivilegesArray( int Value )
		{
			string [] values;
			string value_list;
			ChannelPrivileges cp_mask = (ChannelPrivileges)Value;

			value_list = cp_mask.ToString("G");
			values = value_list.Split( ',' );

			for ( int i = 0; i < values.Length; i++ )
				values[i] = values[i].Trim();

			return values;
		}
		public static string [] UserPrivilegesArray( int Value )
		{
			string [] values;
			string value_list;
			UserPrivileges up_mask = (UserPrivileges)Value;

			value_list = up_mask.ToString("G");
			values = value_list.Split( ',' );

			for ( int i = 0; i < values.Length; i++ )
				values[i] = values[i].Trim();

			return values;
		}
		public static string [] PlayerFlagsArray( int Value )
		{
			string [] values;
			string value_list;
			PlayerFlags pf_mask = (PlayerFlags)Value;

			value_list = pf_mask.ToString("G");
			values = value_list.Split( ',' );

			for ( int i = 0; i < values.Length; i++ )
				values[i] = values[i].Trim();

			return values;
		}
		public static string [] ChannelFlagsArray( int Value )
		{
			string [] values;
			string value_list;
			ChannelFlags cf_mask = (ChannelFlags)Value;

			value_list = cf_mask.ToString("G");
			values = value_list.Split( ',' );

			for ( int i = 0; i < values.Length; i++ )
				values[i] = values[i].Trim();

			return values;
		}
		public static string [] ServerTypeArray( int Value )
		{
			string [] values;
			string value_list;
			ServerType st_mask = (ServerType)Value;

			value_list = st_mask.ToString("G");
			values = value_list.Split( ',' );

			for ( int i = 0; i < values.Length; i++ )
				values[i] = values[i].Trim();

			return values;
		}

		#endregion
	}
}
