using System;
using System.IO;
using System.Text;

namespace ControleRobot
{
    public class GCodeReader
    {
        public GCodeReader()
        {
            // Lin posx posy   posz speed   extrusion speed
            // G1 X-9.2 Y-5.42 Z0.5 F3000.0 E0.0377
            string[] gcode = LireFichier("test.txt");
            for(int i = 0; i < gcode.Length; i++)
            {
                Console.Write(gcode[i] + " FIN LIGNE\n");
            }
        }

        /// <summary>
        /// Ouvre un fichier et renvoie le chaque ligne dans un tableau de string.
        /// </summary>
        /// <param name="chemin">Chemin et nom du fichier</param>
        /// <returns>Contenu du fichier</returns>
        public string[] LireFichier(string chemin)
        {
            return File.ReadAllLines(chemin, Encoding.UTF8);
        } 
    }
}