**NOTE: The repository has moved to GitHub: http://github.com/evanmiller/mod_zip**

**mod\_zip** for nginx assembles ZIP archives dynamically. It can stream component files from
upstream servers with nginx's native proxying code, so that the process never
takes up more than a few KB of RAM at a time, even while assembling archives that
are (potentially) hundreds of megabytes.

Download either from [SVN](http://code.google.com/p/mod-zip/source/checkout) or a [tar-ball](http://code.google.com/p/mod-zip/downloads/list).

To install, compile nginx with the following option:

```
  --add-module=/path/to/mod_zip
```

nginx 0.7.25 or later is required.

The module is activated when the original response (presumably from an
upstream) includes the following HTTP header:

```
  X-Archive-Files: zip
```

It then scans the response body for a list of files. The syntax is a
space-separated list of the file checksum (CRC-32), size (in bytes), location
(properly URL-encoded), and file name. One file per line.  The file location
corresponds to a location in your nginx.conf; the file can be on disk, from an
upstream, or from another module.  The file name can include a directory path,
and is what will be extracted from the ZIP file. Example:

```
1034ab38 428    /foo.txt   My Document1.txt
83e8110b 100339 /bar.txt   My Other Document1.txt
```

Files are retrieved and encoded in order. If a file cannot be found or the file
request returns any sort of error, the download is aborted.

The CRC-32 is optional. Put "-" if you don't know the CRC-32; note that in this
case mod\_zip will disable support the "Range" header.

Tip: add a header "Content-Disposition: attachment; filename=foobar.zip" in the
upstream response if you would like the client to name the file "foobar.zip"