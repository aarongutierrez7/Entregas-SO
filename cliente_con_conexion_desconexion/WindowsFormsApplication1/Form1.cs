using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Net;
using System.Net.Sockets;

namespace WindowsFormsApplication1
{
    public partial class Form1 : Form
    {
        Socket server;
        bool conect = false;
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)//establecer conexion
        {
            //Creamos un IPEndPoint con el ip del servidor y puerto del servidor 
            //al que deseamos conectarnos
            IPAddress direc = IPAddress.Parse("192.168.56.101");
            IPEndPoint ipep = new IPEndPoint(direc, 9050);


            //Creamos el socket 
            server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            try
            {
                server.Connect(ipep);//Intentamos conectar el socket
                this.BackColor = Color.Orange;
                MessageBox.Show("Conectado");
                conect = true;
            }
            catch (SocketException ex)
            {
                //Si hay excepcion imprimimos error y salimos del programa con return 
                MessageBox.Show("No he podido conectar con el servidor");
                return;
            }

        }


        private void button3_Click(object sender, EventArgs e)//desconexion
        {
            if (conect == false)
                MessageBox.Show("NO HAY CONEXION");
            else
            {
                //Mensaje de desconexión
                string mensaje = "0/";

                byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
                server.Send(msg);

                // Nos desconectamos
                this.BackColor = Color.Gray;
                server.Shutdown(SocketShutdown.Both);
                server.Close();
            }
        }

        private void button5_Click(object sender, EventArgs e)//CONSUTAS
        {
            if (conect == false)
                MessageBox.Show("NO HAY CONEXION");
            else
            {
                if (tBcons.Text == "")
                    MessageBox.Show("introduce datos necesarios para realizar consulta");
                else
                {
                    if (radioButton1.Checked)//CONSULTA 1 numero de letras 
                    {
                        string mensaje = "1/" + tBcons.Text;
                         // Enviamos al servidor el nombre tecleado
                        byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
                        server.Send(msg);

                            //Recibimos la respuesta del servidor
                        byte[] msg2 = new byte[80];
                        server.Receive(msg2);
                        mensaje = Encoding.ASCII.GetString(msg2).Split('\0')[0];
                        MessageBox.Show("numero de letras en el nombre " + mensaje);
                        
                    }
                    else if (radioButton2.Checked)//nombre en mayusculas
                    {
                        string mensaje = "2/" + tBcons.Text;
                        // Enviamos al servidor el nombre tecleado
                        byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
                        server.Send(msg);

                        //Recibimos la respuesta del servidor
                        byte[] msg2 = new byte[80];
                        server.Receive(msg2);
                        mensaje = Encoding.ASCII.GetString(msg2).Split('\0')[0];
                        MessageBox.Show("el nombre en mayusculas "+mensaje);
                    }

                    else if (radioButton3.Checked)// 
                    {
                        string mensaje = "3/" + tBcons.Text;
                        // Enviamos al servidor el nombre tecleado
                        byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
                        server.Send(msg);

                        //Recibimos la respuesta del servidor
                        byte[] msg2 = new byte[80];
                        server.Receive(msg2);
                        mensaje = Encoding.ASCII.GetString(msg2).Split('\0')[0];
                        if(mensaje =="0")
                            MessageBox.Show("añadid@");
                        else
                            MessageBox.Show("error, lista llena, no añadid@");
                    }
                    else
                        MessageBox.Show("selecciona cosulta");
                }
            }

        }


        private void radioButton1_MouseHover(object sender, EventArgs e)
        { tBcons.Text = "cual es el numero de letras en el nombre"; }

        private void radioButton1_MouseLeave(object sender, EventArgs e)
        { tBcons.Text = ""; }

        private void radioButton2_MouseHover(object sender, EventArgs e)
        { tBcons.Text = "retorna el nombre en mayusculas"; }

        private void radioButton2_MouseLeave(object sender, EventArgs e)
        { tBcons.Text = ""; }

        private void radioButton3_MouseHover(object sender, EventArgs e)
        { tBcons.Text = "introduce usuario a lista de conectados"; }

        private void radioButton3_MouseLeave(object sender, EventArgs e)
        { tBcons.Text = ""; }


        private void button6_Click(object sender, EventArgs e)//lista conectados
        {
            if (conect == false)
                MessageBox.Show("NO HAY CONEXION");
            else
            {
                string mensaje = "4";
                // Enviamos al servidor el nombre tecleado
                byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
                server.Send(msg);

                //Recibimos la respuesta del servidor
                byte[] msg2 = new byte[80];
                server.Receive(msg2);
                mensaje = Encoding.ASCII.GetString(msg2).Split('\0')[0];
                MessageBox.Show(mensaje);
            }
        }
    }
}
