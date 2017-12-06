# vcv-link

## Link ##
The link module is a very simple module which provides a 'link' connection with a link enabled application like Ableton Live. It will allow you also to connect two or more VCV rack instances on different computers together which are connected in the same WLAN or LAN. In order to sync perfectly we added a Offset control so you can compensate for any latency introduced in the Link connection.

## Installation under Windows ##
The Link module will not work out of the box on Windows due to the difference in build systems of VCV Rack and the Link Library. We provide you with a special DLL which needs to be copied manually to the folder where you installed VCV Rack. After extracting the archive, copy the link-wrapper.dll to the installation folder of VCV Rack(eg C:\Program Files\VCV\Rack). Then restart Rack and the Link module will show up under Stellare Modular.

If the module still does not show up after executing the above steps then you probably need to install the Visual Studio 2015 Redistributable and the .NET Framework 4.71 or 4.72.

![Link](./link.png)

For Enzo and Sander to have inspirational beer drinking sessions in Berlin to come up with brand new groovy ideas for new Rack modules we would appreciate donations, please use the link below.

[Donate through Paypal](https://paypal.me/stellaremodular)
