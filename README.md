### Bag (C lang variant)
Bag is a stupidly simple archive format. 
It stores files inside as simple as possible, nothing more.

### Archive file format
`16-bit big endian` - file path length

`UTF-8` - file path

`64-bit big endian` - file size

`RAW` - file data

### Usage
##### First option must be a mode specifier:

  `-c` Create  `-s` Scan  `-x` Extract
  
##### Common Options:

  `-f <filename>`  Location of archive
  
##### Create: `bag -c [options] [-C <dir> | <dir>]`

  `-C <dir>`  Change to `<dir>` before processing remaining files
  
  `<dir>`  add these items to archive
  
##### List: `bag -s [options] [<pattern>]`

  `<pattern>`  If specified, list only entries that match
  
##### Extract: `bag -x [options] [-C <dir> | <dir> | <pattern>]`

  `-C <dir>`  Change to <dir> before processing remaining files
  
  `<dir>`  extract items from archive to this directory
  
  `<pattern>`  If specified, extract only entries that match
