package com.jbridge;

/**
 * Imprime a propriedade "java.home"
 * que é usada para a jbridge.ddl achar a instalação
 * da jre padrão do sistema.
 *
 */
public class JavaHome {
	public static void main(String[] args) {
		System.out.println(System.getProperty("java.home"));
	}
}
