package com.htssoft.simmons.test;

import com.htssoft.simmons.Spawn;

public class SpawnTest {
	public static void main(String[] args){
		String testProgram;
		if (Spawn.isUnix()){
			testProgram = "/usr/bin/yes";
		}
		else {
			testProgram = "C:\\WINDOWS\\explorer.exe";
		}
		
		Spawn.spawn(testProgram, new String[]{});
	}
}
