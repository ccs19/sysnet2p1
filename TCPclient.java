/**
 * Created by Christopher Schneider on 1/17/15.
 */
import java.io.*;
import java.net.*;



public class TCPClient {
    public static void main(String args[]) throws Exception
    {
        if(args.length != 2)
        {
            System.out.println("Usage: TCPclient <hostname> <args>");
        }
        else
        {
            InetAddress address = InetAddress.getByName(args[0]);
            int port = Integer.parseInt(args[1]);
            System.out.println("IP: " + args[0]);
            Socket clientSock = new Socket(address, port);

            PrintWriter out = new PrintWriter(clientSock.getOutputStream(), true);
            BufferedReader in = new BufferedReader(new InputStreamReader(clientSock.getInputStream()));
            BufferedReader read = new BufferedReader(new InputStreamReader(System.in));

            System.out.println("Input text to send: ");
            String textToSend = read.readLine();
            textToSend = textToSend + '\0';
            out.print(textToSend);
            out.flush();

            //Response
            String response = in.readLine();
            System.out.println(response);

            //clean up
            out.close();
            in.close();
            read.close();
            clientSock.close();

        }
    }






}
