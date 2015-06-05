#!/usr/bin/python

import sys
import os
#==================================================================================================
# Current limitation: Cannot parse a multiline tr("") section.
# This script should be run onlyl once to modify the translatable text.
# In the case of mess up, revert all the changes and try again.
#==================================================================================================
def main():

  fw = open("parse.txt", "w")

  dryRun = True
  dryRun = False

  for root, dirs, files in os.walk(sys.argv[1]):
    for sFile in files:
      fName = os.path.join(root, sFile)
      if "." in fName:
        ext = fName.split(".")[1].lower()
      else:
        continue

      if not(ext == "cpp" or ext == "h"):
        continue

      #print "===================="
      #print fName
      
      with open(fName, "r") as f:
        lines = f.read().split("\n")

      if dryRun:
        pass
      else:
        fw2 = open(fName, "w")

      lineNum = 1

      for line in lines:

        if 'tr("' in line:
          pos1 = line.find("tr(") + 3
          pos2 = line.find('")', pos1 + 1) + 1
          if pos2 <= 0:
            pos2 = line.find('"', pos1 + 1) + 1

          if pos2 - pos1 < 2:
            print "Tricky line:", pos1, pos2, line

          '''
          i = 1
          while True:
            ch = line[pos1 + i: pos1 + i + 1]
            if ch.isalpha() == False and i == 1:
              print line
            else:
              break

            if i == len(line) - 2:
              break
          
            i += 1
          '''

          ch = line[pos1 + 1: pos1 + 2]

          string = line[pos1 + 1:pos2 - 1]
          newString = string

          if ch.upper() != ch:
            newString = string[:1].upper() + string[1:]
            #print newString

          newString = newString.replace("Verilog-a", "Verilog-A")
          newString = newString.replace("DB", "dB")
          newString = newString.replace("Content of project directory", "Contents of project directory")
          newString = newString.replace("Content of current project", "Contents of current project")
          newString = newString.replace("By Michael Margraf", "by Michael Margraf")

          newString = newString.replace("AdmsXml", "ADMSXml")
          newString = newString.replace("Vhdl", "VHDL")
          newString = newString.replace("Npn", "NPN")
          newString = newString.replace("Pnp", "PNP")
          newString = newString.replace("Rms", "RMS")
          newString = newString.replace("Dc Block", "DC block")
          newString = newString.replace("Dc block", "DC block")
          newString = newString.replace("Ac ", "AC ") # Use dry run first
          newString = newString.replace("Ac simulation", "AC simulation")
          newString = newString.replace("dc current", "DC current")
          #newString = newString.replace("Ampere", "amperes")
          newString = newString.replace("Dc Current Source", "DC current source")

          newString = newString.replace("Width of a gap", "Width of the gap")
          newString = newString.replace("Name of component in library", "Name of the component in library")
          newString = newString.replace("Width of gap at end of line", "Width of the gap at end of line")

          newString = newString.replace("qucs ", "Qucs ")
          newString = newString.replace("qucs,", "Qucs,")
          newString = newString.replace("Set to Qucs", "Set to qucs")

          newString = newString.replace("Ending time of the pulse", "End time of the pulse")
          newString = newString.replace("of dielectric", "of the dielectric")
          newString = newString.replace("of shield", "of the shield")
          newString = newString.replace("of conductor", "of the conductor")
          newString = newString.replace("of inner conductor", "of the inner conductor")
          newString = newString.replace("Bias t", "Bias tee")
          newString = newString.replace("of insulator", "of the insulator")
          newString = newString.replace("Ideal rectangle voltage source", "Ideal rectangular voltage source")
          newString = newString.replace("Display in schematic", "Display in the schematic")
          newString = newString.replace("Put operating points into dataset", "Put operating points into the dataset")

          newString = newString.replace("Ohms", "ohms")
          newString = newString.replace("Hertz", "hertz")
          newString = newString.replace("in Amperes", "in amperes")
          newString = newString.replace("in degree", "in degrees")
          newString = newString.replace("degreess", "degrees")
          newString = newString.replace("Volts", "volts")
          newString = newString.replace("in Farad", "in farads")
          newString = newString.replace("in Henry", "in henries")

          newString = newString.replace("ac voltage", "AC voltage")
          newString = newString.replace("degree Celsius", "degrees Celsius")
          newString = newString.replace("Dc Voltage Source", "DC voltage source")
          newString = newString.replace("Name of the s parameter file", "Name of the S-parameter file")
          newString = newString.replace("N-port S parameter file", "N-port S-parameter file")
          newString = newString.replace("Dc simulation", "DC simulation")
          newString = newString.replace("Dc feed", "DC feed")
          newString = newString.replace("These components ?", "These components?")
          newString = newString.replace("A[i]", "a[i]")
          newString = newString.replace("B[i]", "b[i]")
          newString = newString.replace("Magnitude/angle (degree)", "Magnitude/angle (degrees)")
          newString = newString.replace("Magnitude/angle (radian)", "Magnitude/angle (radians)")

          newString = newString.replace("File based", "File-based")
          newString = newString.replace("Bulk junction emission coefficient", "Bulk junction-emission coefficient")
          newString = newString.replace("Coplanar step", "Co-planar step")
          
          if newString == "Simulation temperature":
            newString = "Simulation temperature in degrees Celsius"

          if string != newString:
            fw.write(string + "\n")
            fw.write(newString + "\n\n")

          line = line[:pos1 + 1] + newString + line[pos2 - 1:]

        if not dryRun:
          fw2.write(line)
          if lineNum < len(lines):
            fw2.write("\n")
          
          lineNum += 1

      if not dryRun:
        fw2.close()

  fw.close()
#==================================================================================================
if __name__ == "__main__":
  main()
#==================================================================================================
