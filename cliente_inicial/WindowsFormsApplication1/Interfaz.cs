using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace WindowsFormsApplication1
{
    public partial class Interfaz : Form
    {
        public Interfaz()
        {
            InitializeComponent();
            pictureBox1.Image = new Bitmap(pictureBox1.Width, pictureBox1.Height);
            Graphics graphics = Graphics.FromImage(pictureBox1.Image);
            Brush brush = new SolidBrush(Color.Red);
            graphics.FillRectangle(brush, new System.Drawing.Rectangle(0, 0, 50, 50));

        }

        private void Interfaz_Load(object sender, EventArgs e)
        {
            this.BackgroundImage = new Bitmap("grid.png");
            this.BackgroundImageLayout = ImageLayout.Stretch;
            timer1.Enabled = true;
            timer1.Interval = 1;
            timer1.Start();
        }

        private void timer1_Tick(object sender, EventArgs e)
        {
            Point mouse = PointToClient(MousePosition);
            pictureBox1.Location = new Point(mouse.X, mouse.Y);
        }




    }
}
