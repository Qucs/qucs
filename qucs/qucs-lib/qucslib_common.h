/***************************************************************************
                               qucslib.h
                              -----------
    begin                : Thur Jan 30 2014
    copyright            : (C) 2014 by Richard Crozier
    email                : richard DOT crozier AT yahoo DOT co DOT uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _QUCSLIB_COMMON_H_
#define _QUCSLIB_COMMON_H_

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QDebug>

// global functions and data structures for the processing of
// qucs library files

enum LIB_PARSE_RESULT { QUCS_COMP_LIB_OK,
                        QUCS_COMP_LIB_IO_ERROR,
                        QUCS_COMP_LIB_CORRUPT,
                        QUCS_COMP_LIB_EMPTY };

enum LIB_PARSE_WHAT { QUCS_COMP_LIB_HEADER_ONLY,
                      QUCS_COMP_LIB_FULL };

struct ComponentLibraryItem
{
    QString name;
    QString definition;
    QString symbol;
    QString modelString;
} ;

struct ComponentLibrary
{
    QString name;
    QString defaultSymbol;
    QList<ComponentLibraryItem> components;
} ;

// convert relative path to system library path
// absolute paths (user libs) remain unchanged
inline QString getLibAbsPath(QString libPath)
{
    QDir libdir(QucsSettings.LibDir); // system libraries paths
    QString libAbsPath = libdir.absoluteFilePath(libPath + ".lib");
    return libAbsPath;
}

// gets the contents of a section from a component description
//
// sections are between <secname> </secname> pairs
inline bool getSection(QString section, QString &list, QString &content)
{
    int Start, End;
    Start = list.indexOf("<"+section+">");
    content = "";
    if(Start > 0)
    {
        Start += section.length()+2;
        End = list.indexOf("</"+section+">", Start);
        if(End < 0)
        {
            return false;
        }
        content = list.mid(Start, End-Start);
        content.replace(QRegExp("\\n\\x20+"), "\n").remove(0, 1);
    }
    return true;
}


inline bool getCompLineIntegers(const QString& s,
                                int *i1=0, int *i2=0, int *i3=0, int *i4=0, int *i5=0, int *i6=0)
{
    bool ok;
    QString n;

    if(!i1) return true;
    n  = s.section(' ',1,1);
    *i1 = n.toInt(&ok);
    if(!ok) return false;

    if(!i2) return true;
    n  = s.section(' ',2,2);
    *i2 = n.toInt(&ok);
    if(!ok) return false;

    if(!i3) return true;
    n  = s.section(' ',3,3);
    *i3 = n.toInt(&ok);
    if(!ok) return false;

    if(!i4) return true;
    n  = s.section(' ',4,4);
    *i4 = n.toInt(&ok);
    if(!ok) return false;

    if(!i5) return true;
    n  = s.section(' ',5,5);
    *i5 = n.toInt(&ok);
    if(!ok) return false;

    if(!i6) return true;
    n  = s.section(' ',6,6);
    *i6 = n.toInt(&ok);
    if(!ok) return false;

    return true;
}


// constructs the model string used to paste component libraries
// into a schematic
//
// returns an empty string if it couldn't be constructed
inline int makeModelString (QString libPath, QString compname, QString compstring, QString &modelstring, QString default_sym)
{

    if (!getSection("Model", compstring, modelstring))
    {
        return QUCS_COMP_LIB_CORRUPT;
    }

    // check for a single component line
    if(!modelstring.isEmpty())
    {
        if(modelstring.count('\n') < 2)
        {
            modelstring = modelstring.remove('\n');
            return QUCS_COMP_LIB_OK;
        }
    }

    // The model wasn't a single line so we have to pick through the
    // symbol definition to get the ID for the model string
    QString symbolSection;
    if (!getSection("Symbol", compstring, symbolSection))
    {
        return QUCS_COMP_LIB_CORRUPT;
    }
    if(symbolSection.isEmpty())
    {   // component definition contains no symbol, use library default
        if (default_sym.isEmpty())
        {   // library does not define a default symbol
           return QUCS_COMP_LIB_CORRUPT;
        }
        symbolSection = default_sym; // use library default symbol
    }

    QStringList symbolstringLines = symbolSection.split ("\n");

    QString Prefix = "";
    int Text_x = 0;
    int Text_y = 0;

    for (int i = 0; i < symbolstringLines.count (); i++)
    {
        // remove white space from start and end of line
        symbolstringLines[i] = symbolstringLines[i].trimmed ();

        if(symbolstringLines[i].isEmpty()) continue;

        // check for and strip the surrounding < >, returning an empty
        // string if they're not found
        if(symbolstringLines[i].at(0) != '<')
        {
            return QUCS_COMP_LIB_CORRUPT;
        }
        if(symbolstringLines[i].at(symbolstringLines[i].length()-1) != '>')
        {
            return QUCS_COMP_LIB_CORRUPT;
        }

        // cut off start and end character
        symbolstringLines[i] = symbolstringLines[i].mid(1, symbolstringLines[i].length()-2);

        // get the first statement which contains the component type
        QString s = symbolstringLines[i].section(' ',0,0);
        // check if it's the ID
        if(s == ".ID")
        {

            if (!getCompLineIntegers(symbolstringLines[i], &Text_x, &Text_y))  return QUCS_COMP_LIB_OK;

            Prefix = symbolstringLines[i].section(' ',3,3);

            if(Prefix.isEmpty())
            {
                Prefix = "X";
            }

            break;
        }

    }

    // construct the library model string
    modelstring =  "<Lib " + Prefix + " 1 0 0 " +
                   QString::number(Text_x) + " " +
                   QString::number(Text_y) + " 0 0 \"" +
                   libPath + "\" 0 \"" + compname + "\" 0>";

    return QUCS_COMP_LIB_OK;

}

inline int parseComponentLibrary (QString libPath, ComponentLibrary &library, LIB_PARSE_WHAT what = QUCS_COMP_LIB_FULL)
{
    int Start, End, NameStart, NameEnd;

    QString filename = getLibAbsPath(libPath);

    QFile file (filename);

    if(!file.open(QIODevice::ReadOnly))
    {
        return QUCS_COMP_LIB_IO_ERROR;
    }

    // Read the whole library file into a string then close it
    QTextStream ReadWhole(&file);
    QString LibraryString = ReadWhole.readAll();
    file.close();

    LibraryString.replace(QRegExp("\\r\\n"), "\n");

    // The libraries have a header statement like the following:
    //
    // <Qucs Library 0.0.18 "libname">
    //
    // First do some checking on this statement
    //
    // Check for presence of correctly formatted header statement
    // if it's not present assume it is corrupt and exit
    Start = LibraryString.indexOf("<Qucs Library ");
    if(Start < 0)
    {
        return QUCS_COMP_LIB_CORRUPT;
    }
    // The library was ok so far, look for the closing > of the header
    End = LibraryString.indexOf('>', Start);
    // If the closing > is not found assume corrupt and exit
    if(End < 0)
    {

        return QUCS_COMP_LIB_CORRUPT;
    }
    // Still ok, so try to extract the library name
    library.name = LibraryString.mid(Start, End-Start).section('"', 1, 1);
    // look for nex opening brace of a tag sequence
    Start = LibraryString.indexOf("\n<", End);

    if(Start < 0)
    {
        // nothing else found, library is empty
        return QUCS_COMP_LIB_EMPTY;
    }

    // libraries can have a default symbol section, parse this
    if(LibraryString.mid(Start+2, 14) == "DefaultSymbol>")
    {
        End = LibraryString.indexOf("\n</DefaultSymbol>");
        if(End < 0)
        {
            return QUCS_COMP_LIB_CORRUPT;
        }
        // copy the contents of default symbol section to a string
        library.defaultSymbol = LibraryString.mid(Start+16, End-Start-16);
        Start = End + 3;
    }

    if (what == QUCS_COMP_LIB_HEADER_ONLY)
    {
        // only the header was requested, stop here
        return QUCS_COMP_LIB_OK;
    }

    // Now go through the rest of the component library, extracting each
    // component name
    while((Start=LibraryString.indexOf("\n<Component ", Start)) > 0)
    {
        Start++;
        NameStart = Start + 11;
        NameEnd = LibraryString.indexOf('>', NameStart);
        if(NameEnd < 0)  continue;

        End = LibraryString.indexOf("\n</Component>", NameEnd);
        if(End < 0)  continue;
        End += 13;

        ComponentLibraryItem component;

        component.name = LibraryString.mid(NameStart, NameEnd-NameStart);
        component.definition = LibraryString.mid(Start, End-Start);

        // construct model string
        int result = makeModelString (libPath, component.name, component.definition, component.modelString, library.defaultSymbol);
        if (result != QUCS_COMP_LIB_OK) return result;

        library.components.append (component);

        Start = End;
    }

    return QUCS_COMP_LIB_OK;

}

#endif // _QUCSLIB_COMMON_H_
