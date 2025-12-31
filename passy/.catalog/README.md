
## Tested with the following countries:
🇺🇸
🇨🇱
🇫🇷
🇬🇧
🇵🇭
🇷🇺
🇹🇼
🇺🇦
🇦🇿
🇨🇦
🇮🇹
🇪🇸
🇪🇪
🇨🇭
🇦🇺
🇭🇰
🇦🇹
🇳🇱
🇬🇪
🇰🇷
🇨🇳
🇸🇬
🇧🇩
🇧🇬
🇯🇵
🇬🇷
🇨🇷
🇹🇷
🇲🇽

(If it works for yours, submit a PR to add your country flag)

## To use:

eMTRD are secured to prevent people from reading the data on a passport just by bumping into it.  The data is secured using a key based on the passport number, date of birth, and date of expiry.  A real passport machine reads these values from the MRZ (Machine Readable Zone, the ones with ">") using a camera. For the app, you have to enter the values manually.  The app will then generate the key and read the data using a system called BAC(https://en.wikipedia.org/wiki/Basic_access_control).

## Limitations
 - Avanced menu hidden unless Debug enabled
 - Does not parse some DG under "advanced" menu

## To do
 - Support PACE
 - Support more countries passports

