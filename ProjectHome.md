PPP over jabber tool allows to set up point-to-point connection between 2 machines over jabber protocol.

Original version: http://www.linux.org.ru/gallery/screenshots/2445958

Usage example:

On gateway:
> pppoj gateway@example.com gateway-password client@example.com -master

On client machine:
> pppoj client@example.com client-password gateway@example.com -slave