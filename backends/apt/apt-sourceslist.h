/* apt-sourceslist.h - access the sources.list file
 *
 * Copyright (c) 1999 Patrick Cole <z@amused.net>
 *           (c) 2002 Synaptic development team
*            (c) 2018-2025 Matthias Klumpp <matthias@tenstral.net>
 *
 * Author: Patrick Cole <z@amused.net>
 *         Michael Vogt <mvo@debian.org>
 *         Gustavo Niemeyer <niemeyer@conectiva.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef APT_SOURCESLIST_H
#define APT_SOURCESLIST_H

#include <apt-pkg/fileutl.h>
#include <apt-pkg/tagfile.h>
#include <apt-pkg/metaindex.h>

#include <string>
#include <list>

using namespace std;

class SourcesList {
public:
    enum RecType {
        Deb = 1 << 0,
        DebSrc = 1 << 1,
        Disabled = 1 << 2,
        Comment = 1 << 3,
    };

    struct SourceRecord {
        unsigned int Type;
        string VendorID;
        string PrimaryURI;
        std::vector<std::string> URIs;
        string Dist;
        string *Sections;
        unsigned short NumSections;
        string Comment;

        string SourceFile;
        uint Deb822StanzaIdx;

        string joinedSections(const std::string &separator = " ");
        string niceName();
        string repoId();
        bool hasSection(const char *component);

        bool SetType(string);
        string GetType();
        bool SetURI(string);
        bool SetURIs(const std::vector<std::string> &newURIs);

        SourceRecord():Type(0), Sections(0), NumSections(0), Deb822StanzaIdx(0) {}
        ~SourceRecord() {
            if (Sections) {
                delete [] Sections;
            }
        }
        SourceRecord &operator=(const SourceRecord &);
    };

    struct VendorRecord {
        string VendorID;
        string FingerPrint;
        string Description;
    };

    list<SourceRecord *> SourceRecords;
    list<VendorRecord *> VendorRecords;

private:
    SourceRecord *AddSourceNode(SourceRecord &);
    VendorRecord *AddVendorNode(VendorRecord &);
    bool OpenConfigurationFileFd(std::string const &File, FileFd &Fd);
    bool ParseDeb822Stanza(const char*Type,
                           pkgTagSection &Tags,
                           unsigned int const stanzaIdx,
                           FileFd &Fd);
    bool UpdateSourceLegacy(const std::string &filename);
    bool UpdateSourceDeb822(const std::string &filename);

public:
    SourceRecord *AddSource(RecType Type,
                            string VendorID,
                            string URI,
                            string Dist,
                            string *Sections,
                            unsigned short count, string SourceFile);
    SourceRecord *AddEmptySource();
    void RemoveSource(SourceRecord *&);
    void SwapSources( SourceRecord *&, SourceRecord *& );
    bool ReadSourceDeb822(string listpath);
    bool ReadSourceLegacy(string listpath);
    bool ReadSourcePart(string listpath);
    bool ReadSourceDir(string Dir);
    bool ReadSources();
    bool UpdateSources();

    VendorRecord *AddVendor(string VendorID,
                            string FingerPrint, string Description);
    void RemoveVendor(VendorRecord *&);
    bool ReadVendors();
    bool UpdateVendors();

    SourcesList() {}
    ~SourcesList();
};

typedef list<SourcesList::SourceRecord *>::iterator SourcesListIter;

ostream &operator <<(ostream &, const SourcesList::SourceRecord &);
ostream &operator <<(ostream &os, const SourcesList::VendorRecord &rec);

#endif
