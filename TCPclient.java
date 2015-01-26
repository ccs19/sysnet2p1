/**
 * Created by Christopher Schneider on 1/17/15.
 */
import java.io.*;
import java.net.*;



public class TCPclient {


        private Socket _socket; // the socket for communication with a server
        private PrintWriter out;
        private BufferedReader in;

        /**
         * Constructs a TCPclient object.
         */
        public TCPclient()
        {

        }

        /**
         * Creates a streaming socket and connects to a server.
         *
         * @param host - the ip or hostname of the server
         * @param port - the port number of the server
         *
         * @return - 0 or a negative number describing an error code if the connection could not be established
         */
        public int createSocket(String host, int port)
        {
            try {
                InetAddress address = InetAddress.getByName(host);
                _socket = new Socket(address, port);
                in = new BufferedReader(new InputStreamReader(_socket.getInputStream()));
                out = new PrintWriter(_socket.getOutputStream(), true);
            }
            catch(Exception e)
            {
                System.out.println("New socket failed");
                return -1;
            }
            return 0;
        }

        /**
         * Sends a request for service to the server. Do not wait for a reply in this function. This will be
         * an asynchronous call to the server.
         *
         * @request - the request to be sent
         *
         * @return - 0, if no error; otherwise, a negative number indicating the error
         */
        public int sendRequest(String request)
        {
            try{
                request = request + '\0';
                out.print(request);
                out.flush();
                System.out.println("Sent: " + request);
            }
            catch(Exception e){
                System.out.println("sendRequest failed");
                return -1;
            }

            return 0;
        }

        /**
         * Receives the server's response. Also displays the response in a
         * clear and concise format.
         *
         * @return - the server's response or NULL if an error occured
         */
        public String receiveResponse()
        {
            String response;
            try{
                response = in.readLine();
                System.out.print("Received: ");
            }
            catch(Exception e){
                System.out.println("receiveResponse() failed");
                return null;
            }
            return response;
        }

        /*
        * Prints the response to the screen in a formatted way.
        *
        * response - the server's response as an XML formatted string
        *
        */
        public static void printResponse(String response)
        {
            System.out.println(response);
        }


        /*
         * Closes an open socket.
         *
        * @return - 0, if no error; otherwise, a negative number indicating the error
         */
        public int closeSocket()
        {
            try{
                in.close();
                out.close();
                _socket.close();
            }
            catch(Exception e){
                System.out.println("closeSocket() failed");
                return -1;
            }
            return 0;
        }

        /**
         * The main function. Use this function for
         * testing your code. We will use our own main function for testing.
         */
        public void main(String[] args)
        {

        }









}
