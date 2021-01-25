# geany-openrelative-plugin
hit shortcut key, paste the file to open which is in path that is relative to project directory,
the file uri typically copied from ag or ack

works in linux/ubuntu

```
make
sudo make install
```

usages
for example in the current project there is a file $PROJECTDIR/app/code/aaa.py

* copy the path relative to the current project `app/code/aaa.py`
* press ctrl-shift-P
* ctrl-v (paste) 
* enter


doesn't work in mac osx at the moment, but another hack is to use the `make custom target` feature in `build` menu
