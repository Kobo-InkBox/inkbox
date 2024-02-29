# InkBox
<img src="https://github.com/Kobo-InkBox/inkbox/blob/master/screenshots/homepage.png?raw=true" width="30%"></img> <img src="https://github.com/Kobo-InkBox/inkbox/blob/master/screenshots/reading.png?raw=true" width="30%"></img> <img src="https://github.com/Kobo-InkBox/inkbox/blob/master/screenshots/library.png?raw=true" width="30%"></img> 
## About InkBox
InkBox is an open-source, Qt-based eBook reader for Kobo devices. It also comes as an entire [native open-source OS](https://github.com/Kobo-InkBox/rootfs) and provides support for a number of devices.
<br>
InkBox features:
- Full integrated KoBox X11 subsystem
- ePUB, PDF, pictures and plain text display support
- Versatile configuration options for reading
- muPDF rendering engine for ePUBs and PDFs
- Wi-Fi support and web browser (NetSurf)
- Encrypted storage with EncFS
- Fast dictionary & local storage search
- Dark mode
- Strict security policy ensuring that only signed software can be run on the device (this, however, can be adapted to your needs by recompiling the [kernel](https://github.com/Kobo-InkBox/kernel))
- Full factory reset option if needed
- Seamless update process
- VNC viewer app
- Search function
- 10 built-in fonts
- Enhanced power management
- Lock screen/passcode option
- User-friendly experience
## How do I install InkBox?
You can download precompiled OS/binaries [here](http://23.163.0.39/bundles/inkbox/native/) and standard Nickel add-ons (outdated, unmaintained) are available [here](http://23.163.0.39/bundles/inkbox/nickel/), although support for them has been stopped until further notice. Huge thanks to [@fermino](https://github.com/fermino) for providing free hosting.
<br>

On Windows, use [balenaEtcher](https://www.balena.io/etcher/) to flash the InkBox image file to the Kobo's SD card.

On Linux and MacOS, [balenaEtcher](https://www.balena.io/etcher/) is the easiest option for command-line-reluctant users.
Alternatively, you can also open a terminal and check the device node of the Kobo's SD card with `fdisk -l`. Then `dd` the image to the SD card like this:

```
xzcat inkbox.xz | dd of=/dev/mmcblk0
```
<b>IMPORTANT:</b> InkBox OS does NOT provide ANY WARRANTY at all and we are not responsible if you brick your device with one of the factory images or update bundles available online. See [legal information](https://github.com/Kobo-InkBox/inkbox/blob/master/LEGAL.md) for more details.

## How do I upgrade InkBox?
To upgrade InkBox, go to http://23.163.0.39/bundles/inkbox/native/update/ and copy the files for your device onto the `.inkbox` folder in the USB storage partition.

**Note**: Prior to version 1.6, InkBox had to be upgraded one version at a time. If your device's factory image ships with version 1.5, you need to closely follow the upgrade instructions [here](http://23.163.0.39/bundles/inkbox/native/update/1.6/HOWTO-Update).

If you install the Nickel add-on, unpack the 'base' archive in the root of the onboard storage, not in any subfolder inside it.
## I want to learn more about this!
I suggest you visit the [wiki](https://github.com/Kobo-InkBox/inkbox/wiki). Feel free to contribute if you think you have something valuable to add.
## How can I contribute?
If you like this project and want to improve it in some way, feel free to fork this repository or [one of the subprojects this organization hosts](https://github.com/Kobo-InkBox), then make a [pull request](https://github.com/Kobo-InkBox/inkbox/pulls). I'll be happy to review it when I have time. You can also join the official [Matrix space](https://matrix.to/#/#inkbox-os-project:matrix.org) or [Discord server](https://discord.com/invite/uSWtWbY23m) if you'd like to chat a bit.
<br><br>
On the other hand, if you don't have the appropriate coding skills or just want to help in some way, feel free to make a donation at my [PayPal account](https://paypal.me/inkboxos/), [LiberaPay](https://liberapay.com/tux-linux/), or via Bitcoin: 3MmXq5o78b4C7ub5sb7F5WVC86jzc6PzSN . I'm a student and motivation has been the only thing that has helped me maintain this project for the last few years. Developing an entire operating system on my own was not an easy task. I have time, but I like to spend it wisely.


## Building

Make sure you have the following requirements installed on your system:
* a c/cpp profiler
* `libsndfile-dev`

Next open the `inkbox.pro` file qith QTCreator