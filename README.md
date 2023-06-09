# ConsolePlayer

## Small application for playing videos in cmd

https://user-images.githubusercontent.com/105949852/223510900-49d529f6-dd3b-4702-80b9-3ae7daf521db.mp4

### Full video - [YouTube: Bad Apple!! but it's Terminal](https://youtu.be/vduQ-QZL6sQ)

----
ConsolePlayer.exe *path* [*type*] [*bs*] [*bsize*] [*size_frame*] 

### Arguments:   
1. **path** - path to the video;
2. **type** - playback/conversion type [optional];  
   1) **d_c** - dynamic conversion, conversion is performed during playback;
   2) **s_c** - static conversion, all frames are converted before playback;
   3) **c_c** - combined conversion, a frame buffer is created, which is replenished in the background stream (default type).
3. **bs** - a flag that can be applied when using a combined conversion (***c_c***), displays the current number of frames in the buffer in the window header (disabled by default) [optional];
4. **bsize** - a flag that can be applied when using a combined conversion (***c_c***), sets the buffer size in the number of frames, example ***b20*** (by default, the buffer is 100) [optional];
5. **size_frame** - sets the size of the resulting image in characters, example ***100 100*** (default is 128 by 128) [optional].
---
### For correct display in cmd.exe you need to change the properties:
* Disable forward scrolling on the terminal tab;
* Disable text output transfer when resizing on the location tab;
* You may need to change the size of the screen buffer on the location tab;
* Maybe nothing will help you and the displayed image will flicker or jump, in which case you need to try changing the frame size (**size_frame**) and cmd properties. Yes, cmd is a very capricious thing. There were cases when the changeable properties were applied only after restarting cmd, but in any case, the first two points are required.




