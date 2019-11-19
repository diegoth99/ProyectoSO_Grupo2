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
using System.Threading;

namespace WindowsFormsApplication1
{
    public partial class R0ja_ : Form
    {
        public R0ja_()
        {
            InitializeComponent();
            CheckForIllegalCrossThreadCalls = false;
            this.contrausuario.PasswordChar = '*';
            this.contrausuarioreg.PasswordChar = '*';
            this.repcontrausuarioreg.PasswordChar = '*';
        }

        Socket server;
        Thread atender;
        private void R0ja__Load(object sender, EventArgs e)
        {
            this.desconectar.Hide();
            this.enviar.Hide();
            this.asesino.Hide();
            this.label11.Hide();
            this.login.Hide();
            this.signin.Hide();
            usuario.ReadOnly = true;
            usuarioreg.ReadOnly = true;
            contrausuario.ReadOnly = true;
            repcontrausuarioreg.ReadOnly = true;
            contrausuarioreg.ReadOnly = true;
            jugador.ReadOnly = true;
            asesino.ReadOnly = true;
           
        }

        private void AtenderServidor()
        {
            while (true)
            {
                //Recibimos la respuesta del servidor
                byte[] msg2 = new byte[80];
                server.Receive(msg2);
                string[] trozos = Encoding.ASCII.GetString(msg2).Split('/');
                int codigo = Convert.ToInt32(trozos[0]);
                string mensaje;
                switch (codigo)
                {
                    case 1: //Registrarse
                        mensaje = trozos[1].Split('\0')[0];
                        if (mensaje == "1")
                        {
                            MessageBox.Show(usuarioreg.Text + ". Se ha registrado con éxito");
                        }
                        else
                        {
                            MessageBox.Show("No se ha podido registrar, usuario ya existente");
                        }
                        break;

                    case 2: //Entrar
                        mensaje = trozos[1].Split('\0')[0];
                        if (mensaje == "1")
                        {
                            MessageBox.Show("Credenciales correctos");
                            this.enviar.Show();
                            usuario.Clear();
                            usuario.ReadOnly=true;
                            usuarioreg.Clear();
                            usuarioreg.ReadOnly = true;
                            contrausuario.Clear();
                            contrausuario.ReadOnly = true;
                            repcontrausuarioreg.Clear();
                            repcontrausuarioreg.ReadOnly = true;
                            contrausuarioreg.Clear();
                            contrausuarioreg.ReadOnly = true;
                            login.Hide();
                            signin.Hide();
                        }
                        else
                        {
                            MessageBox.Show("Credenciales incorrectos");
                        }
                        break;

                    case 3: //Consulta +5 kills
                        mensaje = trozos[1].Split('\0')[0];
                        MessageBox.Show("Partidas de " + jugador.Text + " con más de 5 kills: " + mensaje);
                        break;

                    case 4: //Cuantas veces me ha matado
                        mensaje = trozos[1].Split('\0')[0];
                        MessageBox.Show(asesino.Text + " ha matado " + mensaje + " veces a " + jugador.Text + ".");
                        break;

                    case 5: //Kills totales
                        mensaje = trozos[1].Split('\0')[0];
                        MessageBox.Show("Los kills totales de " + jugador.Text + " son: " + mensaje);
                        break;

                    case 6: //Notificacion
                        int longitud = Convert.ToInt32(trozos[1]);
                        label12.Text = "Los conectados son: ";
                        string [] partes=trozos[2].Split('_');
                        for (int a = 0; a < longitud; a++)
                        {
                        label12.Text = label12.Text + partes[a]+", ";                       
                        }
                        break;
                }
            }
        }




        private void conectar_Click(object sender, EventArgs e)
        {
            IPAddress direc = IPAddress.Parse("147.83.117.22");
            IPEndPoint ipep = new IPEndPoint(direc, 50054);

            //Creamos el socket 
            server = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            try
            {
                server.Connect(ipep);//Intentamos conectar el socket
                this.conectar.Hide();
                this.desconectar.Show();
                this.login.Show();
                this.signin.Show();
                usuario.Clear();
                usuario.ReadOnly = false;
                usuarioreg.Clear();
                usuarioreg.ReadOnly = false;
                contrausuario.Clear();
                contrausuario.ReadOnly = false;
                repcontrausuarioreg.Clear();
                repcontrausuarioreg.ReadOnly = false;
                contrausuarioreg.Clear();
                contrausuarioreg.ReadOnly = false;
                jugador.ReadOnly = false;
                jugador.Clear();
                asesino.ReadOnly = false;
                asesino.Clear();
            }
            catch {
                MessageBox.Show("Error al conectar");
            }

            ThreadStart ts = delegate { AtenderServidor(); };
            atender = new Thread(ts);
            atender.Start();
        }

        private void desconectar_Click(object sender, EventArgs e)
        {
            string mensaje = "0";
            // Enviamos al servidor el nombre tecleado
            byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
            server.Send(msg);

            // Se terminó el servicio. 
            // Nos desconectamos
            atender.Abort();
            server.Shutdown(SocketShutdown.Both);
            server.Close();
            this.desconectar.Hide();
            this.conectar.Show();
            this.enviar.Hide();
            this.signin.Hide();
            this.login.Hide();
            usuario.Clear();
            usuario.ReadOnly = true;
            usuarioreg.Clear();
            usuarioreg.ReadOnly = true;
            contrausuario.Clear();
            contrausuario.ReadOnly = true;
            repcontrausuarioreg.Clear();
            repcontrausuarioreg.ReadOnly = true;
            contrausuarioreg.Clear();
            contrausuarioreg.ReadOnly = true;
            jugador.ReadOnly = true;
            jugador.Clear();
            asesino.ReadOnly = true;
            asesino.Clear();
            label12.Text="";
        }

      
        private void consulta4_CheckedChanged(object sender, EventArgs e)
        {
            if (consulta4.Checked == true)
            {
                this.label11.Show();
                this.asesino.Show();
            }
            else {
                this.asesino.Hide();
                this.label11.Hide();
            }
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox1.Checked == true)
            {
                contrausuario.PasswordChar = '\0';
            }
            else
            {
                contrausuario.PasswordChar = '*';
            }
        }

        private void checkBox2_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox2.Checked == true)
            {
                contrausuarioreg.PasswordChar = '\0';
            }
            else
            {
                contrausuarioreg.PasswordChar = '*';
            }
        }

        private void checkBox3_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox3.Checked == true)
            {
                repcontrausuarioreg.PasswordChar = '\0';
            }
            else
            {
                repcontrausuarioreg.PasswordChar = '*';
            }
        }

        private void login_Click(object sender, EventArgs e)
        {
            if (usuario.Text == "" || contrausuario.Text == "")
            {
                MessageBox.Show("Introduce los datos correctamente", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            else
            {
                try
                {
                    string mensaje = "2/" + usuario.Text + "/" + contrausuario.Text;
                    // Enviamos al servidor el nombre tecleado
                    byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
                    server.Send(msg);
                }
                catch (SocketException)
                {
                    //Si hay excepcion imprimimos error y salimos del programa con return 
                    MessageBox.Show("No he podido conectar con el servidor");
                    return;
                }
            }
        }

        private void signin_Click(object sender, EventArgs e)
        {
            if (usuarioreg.Text == "" || contrausuarioreg.Text == "")
            {
                MessageBox.Show("Introduce los datos correctamente", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            else if (contrausuarioreg.Text != repcontrausuarioreg.Text)
            {
                MessageBox.Show("La constraseña no coincide, prueba de nuevo", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
            else
            {
                try
                {
                    string mensaje = "1/" + usuarioreg.Text + "/" + contrausuarioreg.Text;
                    // Enviamos al servidor el nombre tecleado
                    byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
                    server.Send(msg);
                }
                catch (SocketException)
                {
                    //Si hay excepcion imprimimos error y salimos del programa con return 
                    MessageBox.Show("No he podido conectar con el servidor");
                    return;
                }
            }
        }

        private void enviar_Click(object sender, EventArgs e)
        {
            try
            {
                if (consulta3.Checked)
                {
                    if (jugador.Text != "")
                    {
                        string mensaje = "3/" + jugador.Text;
                        // Enviamos al servidor el nombre tecleado
                        byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
                        server.Send(msg);
                    }
                    else
                        MessageBox.Show("Pon algo graciosillo");
                }
                else if (consulta4.Checked)
                {
                    if (jugador.Text != "" && asesino.Text != "")
                    {
                        string mensaje = "4/" + jugador.Text + "/" + asesino.Text;
                        // Enviamos al servidor el nombre tecleado
                        byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
                        server.Send(msg);
                    }
                    else
                        MessageBox.Show("Pon algo graciosillo");
                }
                else if (consulta5.Checked)
                {
                    if (jugador.Text != "")
                    {
                        string mensaje = "5/" + jugador.Text;
                        // Enviamos al servidor el nombre tecleado
                        byte[] msg = System.Text.Encoding.ASCII.GetBytes(mensaje);
                        server.Send(msg);
                    }
                    else
                        MessageBox.Show("Pon algo graciosillo");
                }
                else
                {
                    MessageBox.Show("Selecciona una opción");
                }
            }
            catch (SocketException) {
                //Si hay excepcion imprimimos error y salimos del programa con return 
                MessageBox.Show("No he podido conectar con el servidor");
                return;
            }
        }


    }
}
