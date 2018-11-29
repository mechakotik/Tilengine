/*
* Tilengine - The 2D retro graphics engine with raster effects
* Copyright (C) 2015-2018 Marc Palacios Domenech <mailto:megamarc@hotmail.com>
* All rights reserved
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Library General Public License for more details.
*
* You should have received a copy of the GNU Library General Public
* License along with this library. If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Tilengine.h"
#include "LoadFile.h"

/*!
 * \brief
 * Loads a spriteset from a png/txt file pair
 * 
 * \param name
 * Base name of the files containing the spriteset
 * 
 * \returns
 * Reference to the newly loaded spriteset or NULL if error
 * 
 * \remarks
 * The spriteset comes in a pair of files called name.png and name.txt. The png file contains
 * the spriteset, whereas the txt contains the coordinates of the rectangles that define individual sprites.
 * These files can be created wit the spritesheet packer tool (http://spritesheetpacker.codeplex.com/)
 * \remarks
 * An associated palette is also created, it can be obtained calling TLN_GetSpritesetPalette()
 */
TLN_Spriteset TLN_LoadSpriteset (const char* name)
{
	FILE *pf;
	char filename[64];
	char line[64];
	int entries = 0;
	TLN_Bitmap bitmap;
	TLN_Spriteset spriteset;
	TLN_SpriteData *sprite_data;
	int c;

	/* load png file */
	sprintf (filename, "%s.png", name);
	bitmap = TLN_LoadBitmap (filename);
	if (!bitmap)
		return NULL;

	/* load txt file */
	sprintf (filename, "%s.txt", name);
	pf = FileOpen (filename);
	if (!pf)
	{
		TLN_DeleteBitmap (bitmap);
		TLN_SetLastError (TLN_ERR_FILE_NOT_FOUND);
		return NULL;
	}

	/* count lines */
	while (fgets (line, 64, pf))
		entries++;

	sprite_data = malloc (sizeof(TLN_SpriteData)*entries);
	if (!sprite_data)
	{
		TLN_DeleteBitmap (bitmap);
		TLN_SetLastError (TLN_ERR_OUT_OF_MEMORY);
		fclose (pf);
		return NULL;
	}

	/* read entries */
	fseek (pf, 0, SEEK_SET);
	for (c=0; c<entries; c++)
	{
		char* equals;
		TLN_SpriteData* entry;

		/* lee linea */
		fgets (line, 64, pf);
		entry = &sprite_data[c];

		/* format SpriteSheetPacker: name = x y w h */
		equals = strchr (line, '=');
		if (equals != NULL)
		{
			sscanf (line, "%s = %d %d %d %d", entry->name, &entry->x, &entry->y, &entry->w, &entry->h);
			continue;
		}

		/* format Leshy SpriteSheet Tool csv: name,x,y,w,h */
		equals = strchr (line, ',');
		if (equals != NULL)
		{
			char* del = line;
			while (*del)
			{
				if (*del == ',')
					*del = ' ';
				del++;
			}
			sscanf (line, "%s %d %d %d %d", entry->name, &entry->x, &entry->y, &entry->w, &entry->h);
			continue;
		}
	}
	fclose (pf);

	/* create */
	spriteset = TLN_CreateSpriteset (bitmap, sprite_data, entries);
	
	/* free resources */
	free (sprite_data);
	
	if (spriteset)
		TLN_SetLastError (TLN_ERR_OK);
	else
		TLN_SetLastError (TLN_ERR_OUT_OF_MEMORY);
	
	return spriteset;
}
