# Menu Music Source

`tools/music/menu-music.json` is the editable source for the firmware menu
music. It is normally edited through TableTop Studio's audio piano-roll and
regenerated with `make music-data`.

The checked-in `sources/entertainer.mid` and `sources/entertainer.ly` files are
kept only as optional reference material from the earlier menu theme. They come
from the Mutopia Project edition of Scott Joplin's "The Entertainer".

- Source page: https://www.mutopiaproject.org/cgibin/piece-info.cgi?id=263
- Mutopia source note: reproduction of the original 1902 edition.
- Copyright status on Mutopia: public domain.

Run `make music-data` to regenerate `src/MenuMusic.*` from the editable JSON
source. Use `tools/music/generate_menu_music.py --force-midi` only if the old
Mutopia MIDI source needs to be rebuilt again.
