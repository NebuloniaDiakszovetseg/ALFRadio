Készítõ(k) jegyzetei:

1. az applikació teljes egésszében angolul van írva mivel
 ez programozásban praktikusabb

2. Ha ráviszed a mouse-t az i-gombra akkor megjelenik minden
app-el kapcsolatos kezelési információ

3.Ha megváltoztatod a hangkimeneti eszközt ajánlatos ujraindítani az applikációt
Az app mûködését nem tárgyaltam bekötött hangflalak hiányának esetén(- várhatlan hibát adhat)

4. Az app monóba megy mivel ennek készítésekor a hangfalak is monóba voltak kötve
(tudtommal a külömbözõ emeletek külömbözõ kanálisokon vannak viszont nem próbáltam ki)

5. Az érzékelt mikrofon az elsõ bemeneteli csatornát érzékeli, hibák
elkerüléséért kapcsold ki(disable) a nem használt mikrofonokat
(Figyelmetekre érdemes a WuMic alkalmazás ami telefonról számítógépre vetíti(streameli) a hangot)

6. Ha az aplikáció látszólag lényegtelen okból kifolyólag nem indul el töröld a "settings.json" file-t
	(ezt az app magától újra kellene generálja(az alapvetõ beálításokkal))

7. a "settings.json" file kézzel editálható. Vannak benne eldugott, az app által nem megjelenített
opciók amiket megváltoztathatsz.(Változtatásokat csak akkor végezz ha az app zárva van):
	a.)szünetek változtatásának céljából átírhatod az alapvetõen "0"->"7" json struktúrájú adatokat
		Ez esetben vigyázz hogy a szünetek 0-tól n-1-ig indexelõdjenek
			(pl a 0-dik szünet törlése esetén újra kell nevezni az összes tóbbit az i-1 indexel)
		Az utolsó szünet kezdési ideje "null". Ez jelzi a programnak hogy a szünetek "táblázatának"
		vége. Ezt NE töröld ki!
	b.)ha olyan hiba adódik a programban aminek nem tudod a forrását elindíthatod a konzol-képernyõt.
		Ehhez a "console_hide" értékét alakítsd "false"-á.
		Indításkor az app el kellene indítson a render window mellett még egy console window-t is
		amin kiíródnak a készítõ(k) által tárgyalt hibák.
	c.) "dim_duration" - egéssz számértékben(pl. 1.5 = 90s) megadott másodpercszám.
		Ez jelképezi az idõeltolódást amivel a mikrofonenyítés reagál a "csend"-re.
	stb.
	
Megemlítésre érdemes Ugrai Márton és társai, akik eredetileg bekötötték a rádiót.
Az applikációt készítette:
1.Szõke András-Loránd, 2020 elején
  (Itt a facebookom, ha szükségetek van rám: https://www.facebook.com/szoke.lori)
<neved add meg>
  