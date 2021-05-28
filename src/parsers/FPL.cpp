/*
	This is a modified copy of the following cpp file:
	https://github.com/yellowcrescent/fplreader/blob/master/fplreader.cpp
	
	Copyright (c) 2011-2014 Jacob Hipps
	license		GNU LGPLv3 - http://www.gnu.org/licenses/lgpl.html
*/


#include <parser.h>

#include <stdio.h>
#include <string.h> //strncpy

#if defined(_WIN32)
#include <wchar.h> //_wfopen
#endif

typedef struct {
	unsigned int unk1;		// not sure??
	unsigned int file_ofz;	// filename string offset
	unsigned int subsong;	// subsong index value
	unsigned int fsize;		// filesize
	unsigned int unk2;		// ??
	unsigned int unk3;		// ??
	unsigned int unk4;		// ??
	char         duration_dbl[8]; // track duration data (converted later)
							// note: for some reason, using a double here will cause fread to read 12 bytes instead
							// of 8 (not to mention incorrectly interpreting the double in the first place, with
							// the most significant byte being null)...
							// possibly because of word alignment?? who knows...
	float        rpg_album;	// replay gain, album
	float        rpg_track;	// replay gain, track
	float		 rpk_album;	// replay gain, album peak
	float        rpk_track; // replay gain, track peak
	unsigned int keys_dex;	// number of key/pointers that follow
	unsigned int key_primary; // number of primary info keys
	unsigned int key_second;  // number of secondary info key combos
	unsigned int key_sec_offset; // index of secondary key start

} FPL_TRACK_CHUNK;


typedef struct {
	int key;
	char field_name[128];
	char value[1024];

} FPL_TRACK_ATTRIB;

// track data
FPL_TRACK_CHUNK		chunkrunner;
FPL_TRACK_ATTRIB	trackrunner[256];


const QString formatPOS(fpos_t &fp) {
#if defined(__linux__)
    return QString::number(fp.__pos);
#else
    return QString::number(fp);
#endif
}

bool Parser::parseFPL()
{
    FILE *fplFile = NULL;

    #if defined(_WIN32)
    //It's important to use wide characters (wchar_t) for windows platforms
    //or unicode will not work.
    fplFile = _wfopen(FileName.toStdWString().c_str(), L"rb");
    #else
    fplFile = fopen(FileName.toStdString().c_str(), "rb");
    #endif
    if (fplFile != NULL) {
		// read 16-byte signature
		char magicsig[16];
        fread(magicsig, 16, 1, fplFile);

		// load primary data string into memory
		unsigned int data_sz;
        fread(&data_sz, 4, 1, fplFile);

        emit log(info, tr("Size of primary data area: %1 bytes.").arg(QString::number(data_sz)));
        emit log(info, tr("Now allocating memory for it..."));
		
        char *dataprime;

        if ((dataprime = (char*)malloc(data_sz)) != NULL) {
            emit log(done, tr("Cool, memory allocated successfully, Now loading data to it..."));
			
			// read in primary string to memory
            fread(dataprime, data_sz, 1, fplFile);

			// read playlist count integer
			unsigned int plsize;
            fread(&plsize, 4, 1, fplFile);

            emit log(info, tr("Looks like there's %1 items in this foobar2000 playlist.").arg(QString::number(plsize)));

			// entering chunk reader loop...
			unsigned int		keyrunner[512];
            fpos_t				fploffset;
			double				duration_conv;
			int					real_keys;
			int					attrib_count = 0;
			int					trx_dex = 0;
			char				tdata_fname[1024];

            struct FPL {
                QString filePath, Title, Artist, Album, Duration;
                unsigned long length = 0;
                uint64_t fileSize = 0;
			};
            QVector<FPL> tracks;

            for (unsigned int i = 0; i < plsize && !feof(fplFile); i++) {
                fgetpos(fplFile, &fploffset);
                emit log(info, tr("Currently reading info at %1, Start offset: %2").arg(QString::number(i), formatPOS(fploffset)));

                fread((void*)&chunkrunner, sizeof(FPL_TRACK_CHUNK), 1, fplFile);

                fgetpos(fplFile, &fploffset);
                emit log(info, tr("Alright...done, now ending offset: %1").arg(formatPOS(fploffset)));

				// tricky casting! MSVC won't let us do a char to double cast... so we'll show him who's boss...
                memcpy((void*)&duration_conv, chunkrunner.duration_dbl, 8);

				// attribute count is primary keys (key_primary) + secondary keys (key_second)
				attrib_count = chunkrunner.key_primary + chunkrunner.key_second;

                emit log(info, tr("This track has %1 attribute fields.").arg(QString::number(attrib_count)));
				
				// keys_dex sanity check
                if (chunkrunner.keys_dex <= 512) {
					// read in key values from file
                    fgetpos(fplFile, &fploffset);

					// since we've already read 3 of the "keys" (key_primary,key_second, and key_sec_offset), we subtract 3
					real_keys = chunkrunner.keys_dex - 3;

                    emit log(info, tr("Reading %1 (adjusted) values, Starting offset: %2").arg(QString::number(real_keys), formatPOS(fploffset)));

                    fread((void*)&keyrunner, sizeof(unsigned int), real_keys, fplFile);

                    emit log(info, tr("Ending offset: %1").arg(formatPOS(fploffset)));

					// enumerate track data from keyrunner
					trx_dex = 0;  // reset trackrunner indexer

					// Enumerate primary keys, which contain a key_value->field_name pair (hence, the x2 multiplier).
					// After all the key_value->field_name pairs is a list of values which is preceeded by the
					// key_value which is equal to key_primary's value
                    for (unsigned int ii = 0; ii < (chunkrunner.key_primary * 2); ii += 2) {
						// key value
						trackrunner[trx_dex].key = keyrunner[ii];
						// field name
                        strncpy(trackrunner[trx_dex].field_name,
                        (char*)(dataprime + keyrunner[1 + ii]),
                        sizeof(trackrunner[trx_dex].field_name));
						// value
                        strncpy(trackrunner[trx_dex].value,
                        (char*)(dataprime + keyrunner[1 + trackrunner[trx_dex].key + (chunkrunner.key_primary * 2)]),
                        sizeof(trackrunner[trx_dex].value));
						trx_dex++;
					}

					// enumerate secondary keys, which are field_name->value pairs, with NO key_value, as they are usually
					// additional data that is not used as often
                    for (unsigned int ii = 0; ii < (chunkrunner.key_second * 2); ii += 2) {
						// set the key value as -1 to represent UNDEFINED
						trackrunner[trx_dex].key = -1; 
						// field name
                        strncpy(trackrunner[trx_dex].field_name,
                        (char*)(dataprime + keyrunner[ii+chunkrunner.key_sec_offset]),
                        sizeof(trackrunner[trx_dex].field_name));
						// value
                        strncpy(trackrunner[trx_dex].value,
                        (char*)(dataprime + keyrunner[1+ii+chunkrunner.key_sec_offset]),
                        sizeof(trackrunner[trx_dex].value));
						trx_dex++;
					}

                    emit log(done, tr("Enumerated %1 attributes (expected %2).").arg(QString::number(trx_dex),QString::number(attrib_count)));

                    strncpy(tdata_fname, (char*)(dataprime + chunkrunner.file_ofz), sizeof(tdata_fname)); // get filename string


					FPL fpl;

                    fpl.filePath = QString::fromUtf8(tdata_fname);
                    fpl.Duration = formatLength((unsigned long)duration_conv);
                    fpl.length   = (unsigned long)duration_conv;
					fpl.fileSize = chunkrunner.fsize;


                    if (fpl.filePath.length() > 3) {
                        for (int ii = 0; ii < trx_dex; ii++) {
                            if (strcmp(trackrunner[ii].field_name, "title") == 0)
                                fpl.Title = QString::fromUtf8(trackrunner[ii].value);
							
                            if (strcmp(trackrunner[ii].field_name, "artist") == 0)
                                fpl.Artist = QString::fromUtf8(trackrunner[ii].value);

                            if (strcmp(trackrunner[ii].field_name, "album") == 0)
                                fpl.Album = QString::fromUtf8(trackrunner[ii].value);
						}

                        tracks.push_back(fpl);
					}
				}
                else {
                    emit log(error, tr("Chunkrunner failed the sanity check, "
                    "keys_dex is bigger than 512, keys_dex=%1").arg(QString::number(chunkrunner.keys_dex)));
                    return false;
                }
			}

            if (tracks.size() > 0) {
                emit setPBMax(tracks.size());

                for (auto &track : tracks) {
                    int index((&track - tracks.begin()) + 1);

                    if (_CANCEL_LOADING) {
                        emit log(warn, tr("Cancel signal has been made, Cancelling loading process..."));
                        return false;
                    }

                    if (Performance > 0)
                        QThread::msleep(Performance);

                    emit setPB(index);

					File file;
                    file.ID = generateID();

                    file.name     = BaseName(track.filePath);
                    file.fullPath = track.filePath;

                    file.title  = track.Title;
                    file.artist = track.Artist;
                    file.album  = track.Album;
					
                    file.length      = track.Duration;
                    file.rawLength   = track.length;
                    file.fileSize    = humanizeBytes(track.fileSize);
                    file.rawFileSize = track.fileSize;

                    AddItemToPlaylist(file);
				}
			}
            else {
                emit log(error, tr("Hmmm, looks like this FPL file either empty or corrupted. No files were found so cancelling the loading process."));
                return false;
            }
		}
        else {
            emit log(error, tr("Failed to allocate memory."));
            return false;
        }

        fclose(fplFile);
	}
    else {
        emit log(error, tr("Failed to open/read the FPL file \"%1\".").arg(FileName));
        return false;
    }

    return true;
}
