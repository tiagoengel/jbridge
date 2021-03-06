package com.jbridge;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintWriter;

public class ExceptionPrinter {
	
		
    public static void print(Throwable t) throws IOException {
    	File logFile = new File(System.getProperty("java.io.tmpdir")+"/Jbridge_log.txt");
    	BufferedWriter writer = new BufferedWriter(new FileWriter(logFile));
    	StackWriter stackWriter = new StackWriter(System.out, writer);
    	t.printStackTrace(stackWriter);    
    	writer.close();
    	writer = null;
    	stackWriter.close();
    	stackWriter = null;
    	logFile = null;
    }
    
    static class StackWriter extends PrintWriter {   
        private BufferedWriter writer;
        public StackWriter(OutputStream stream) throws IOException {
            super(stream);             
        }   
        public StackWriter(OutputStream stream, BufferedWriter writer) throws IOException {
            this(stream);
            this.writer = writer;
        }         
        @Override
        public void write(String string) {         
        	System.out.println(string);
        	if (writer != null) {
        		try {
        			writer.write(string);
        			writer.newLine();
        		} catch (IOException e) {				
        			e.printStackTrace();
        		}				
        	}
        }        
    }    
}