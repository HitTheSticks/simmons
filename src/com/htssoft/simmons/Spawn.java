package com.htssoft.simmons;


/**
 * This class offers an entry point for spawning a new process.
 * 
 * The goal of this is to spawn the new process as a detached process from the
 * calling JVM process. If you care, in the slightest little bit, about the output
 * or exit code of the spawned application, this ain't for you.
 * 
 * This will allow a calling process to exit and leave behind all children
 * processes, without a requirement that the calling JVM remain resident to 
 * clear the child process stdout and stderr buffers.
 * 
 * On Windows, this takes the form of a CreateProcess call. On linux and OSX,
 * it is implemented with fork()/setsid()/exec().
 * */
public class Spawn {
	static {
		try {
			System.loadLibrary("simmons");
		}
		catch (Exception ex){
			System.err.println("Cannot load libsimmons.");
			ex.printStackTrace();
		}
	}
	
	public static void spawn(String programPath, String[] arguments){
		if (isUnix()){
			unixSpawn(programPath, arguments);
		}
		else if (isWindows()){
			windowsSpawn(programPath, arguments);
		}
	}
	
	protected static void unixSpawn(String programPath, String[] arguments){
		String[] cStyleArgs = new String[arguments.length + 1];
		cStyleArgs[0] = programPath;
		System.arraycopy(arguments, 0, cStyleArgs, 1, arguments.length);
		backgroundSpawn(programPath, cStyleArgs);
	}
	
	protected static void windowsSpawn(String programPath, String[] arguments){
		String escapedProgName = wrapInQuotes(programPath);
		String joinedArgs = escapedProgName + " " + join(" ", arguments);
		backgroundWindowsSpawn(programPath, joinedArgs);
	}
	
	/**
	 * Spawn a unix process with fork()/setsid()/exec().
	 * */
	private static native void backgroundSpawn(String programPath, String[] arguments);
	
	/**
	 * Spawn a windows program with CreateProcess. 
	 * */
	private static native void backgroundWindowsSpawn(String programPath, String commandLine);
	
	/**
	 * Is this a unix machine?
	 * */
	public static boolean isUnix(){
		String os = System.getProperty("os.name").toLowerCase();
		return (os.contains("mac") || os.contains("nix") || os.contains("nux"));
	}
	
	/**
	 * Is this a windows machine?
	 * */
	public static boolean isWindows(){
		String os = System.getProperty("os.name").toLowerCase();
		return os.contains("win");
	}
	
	/**
	 * Wrap a long windows path in strings.
	 * */
	public static String wrapInQuotes(String inString){
		return String.format("\"%s\"", inString);
	}
	
	/**
	 * Join objects as strings with the given delimiter.
	 * */
	public static String join(String delimiter, String...strings) {
	    if (strings.length == 0){
	    	return "";
	    }
	    
	    StringBuffer buffer = new StringBuffer(strings[0]);
	    for (int i = 1; i < strings.length; i++){
	    	buffer.append(delimiter);
	    	buffer.append(strings[i].toString());
	    }
	    return buffer.toString();
	}
}
