package com.jbridge;

import java.io.IOException;
import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;
import java.util.logging.FileHandler;
import java.util.logging.Logger;
import java.util.logging.SimpleFormatter;

public class Dispatcher {
	
	private static final Logger log = Logger.getLogger("Dispatcher");
	static {
		try {
			FileHandler handler = new FileHandler(System.getProperty("java.io.tmpdir")+"/dispacher.log");
			handler.setFormatter(new SimpleFormatter());
			log.addHandler(handler);
		} catch (IOException e) {
			e.printStackTrace();
		} 
		
	}
	
	private static final String sepArgChar = "@";
	private static Map<String, Class<?>> classMap;
	static {
		classMap = new HashMap<String, Class<?>>();
		classMap.put("String"   , String.class);
		classMap.put("Integer"  , Integer.class);
		classMap.put("Double"   , Double.class);		
		classMap.put("Float"    , Float.class);
		classMap.put("Boolean"  , Boolean.class);
	}
	
	private static Map<Class<?>, Class<?>> primitiveMap;
	static {
		primitiveMap = new HashMap<Class<?>, Class<?>>();		
		primitiveMap.put(int.class    , Integer.class);
		primitiveMap.put(double.class , Double.class);
		primitiveMap.put(char.class   , Character.class);
		primitiveMap.put(float.class  , Float.class);
		primitiveMap.put(long.class   , Long.class);
		primitiveMap.put(byte.class   , Byte.class);
		primitiveMap.put(boolean.class, Boolean.class);
	}
	
	public static void main(String[] args) throws ClassNotFoundException, Exception {
		Dispatcher.dispatch(Class.forName("java.lang.Math"), "random", new String[]{});
	}
	
	/**
	 * Esse metodo converte os parametros para os seus tipos 
	 * especificos, chama a funcao informada e formata o 
	 * retorno 
	 *
	 * @param clazz a <code>Class<code> onde o metodo se encontra 
	 * @param methodName o nome do metodo
	 * @param conn a conexao com o banco, caso exista
	 * @param args os parametros
	 * @param formatter o formato do retorno
	 * @return
	 * @throws Throwable 
	 */
	public static Object dispatch(Class<?> classToCall, String methodName, String args[]) throws Exception {
		log.info(classToCall.getName()+"."+methodName+Arrays.toString(args));
		Object[] objArgs = getJavaArgs(args);	
		Method method = findMethod(classToCall, methodName, objArgs);	
		
		Object ret = method.invoke((Modifier.isStatic(method.getModifiers()) ? classToCall : classToCall.newInstance()),objArgs);	
		return ret;
	}
	
	private static Method findMethod(Class<?> clazz, String methodName, Object[] args) throws Exception{
		Method[] methods = clazz.getDeclaredMethods();
		boolean isThis = false;
		
		for (Method m : methods) {
			if (m.getName().equals(methodName)) {				
				Class<?>[] mArgs = m.getParameterTypes();
				if (mArgs.length == args.length) {
					isThis = true;
					for (int i = 0; i < args.length; i++) {
						if (!(mArgs[i].isPrimitive() ? primitiveMap.get(mArgs[i])
								.isAssignableFrom(args[i].getClass()) 
								: mArgs[i].isAssignableFrom(args[i].getClass())))
							isThis = false;
					}
				}
				if (isThis) return m;
			}
		}
		throw new NoSuchMethodException("O metodo "+methodName+"("+formatArgs(args)+") não foi encontrado");
	}
	
	private static Object[] getJavaArgs(String args[]) throws Exception {
		Object[] retorno = new Object[args.length];
		Class<?> clazz = null;
		String value = null;
		for (int i = 0; i < args.length; i++) {
			clazz = classMap.get(args[i].substring(0, args[i].indexOf(sepArgChar)));
			value = args[i].substring(args[i].indexOf(sepArgChar) + 1, args[i].length());
			retorno[i] = castToType(clazz, value);
		}
		return retorno;
	}
	
	private static Object castToType(Class<?> type, String value) throws Exception{
		if (type == String.class)              return value;
		else if (type == Integer.class)        return Integer.parseInt(value);
		else if (type == Double.class)     	   return Double.parseDouble(value);
		else if (type == Float.class)   	   return Float.parseFloat(value);
		else if (type == Boolean.class)   	   return Boolean.parseBoolean(value);
		
		else throw new IllegalArgumentException("Tipo "+(type != null ? type.getSimpleName() +  " não suportado" : "não encontrado"));
	}
	
	private static String formatArgs(Object[] args) {
		StringBuilder sB = new StringBuilder();
		for (int i=0; i<args.length; i++) {
			if (i == args.length - 1)
				sB.append(args[i].getClass().getName());
			else 
				sB.append(args[i].getClass().getName()+", ");
		}
		
		return sB.toString();
	}
}
