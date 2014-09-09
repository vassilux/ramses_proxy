*************************************************
* raRt - 	
* 
* 					
*
*************************************************

Notes:
Instalaltion de boost en version 1.40 ou superieure est requise :
system
thread
program_options

1. Génération de make file 
	1.1 cmake_debug.sh
	1.2 cmake_release.sh
2. Compilation
	2.1 make (standart)
	2.2 make install (par default exécutable dans sera installer dans /usr/bin, configuraiton dans /etc ), il faut utilsier un compte avec les droits d'ecriture dans ces répertoires 
3. Configuration
	3.1 Par défault la fichier de configuraiton kaR.cfg se trouve dans /etc
	3.2 Valeur à initialiser
	f1serveradr=adresse/nom machine du serveur F1 cible
	f1serverport=port du seveur F1
	udpport=port udp de proxy
	tcpport=port tcp de proxy (reservé pour la future)
	tcpsessiontimeout=1800000[n'est pas utilisé, reservé pour la future ]
	3.3 Log level configuraiton. 
	Plase see the file rart_log.conf. The default log file configuration is /ERMO/log/tech.
	See the file configuration section 
	Production level example : 
	[levels]
	TRACE = 80, LOG_CRIT 
	Debug level example : 
	[levels]
	TRACE = 10, LOG_CRIT 

	[rules]
	rart_cat.TRACE      "/ERMO/log/tech/rart.log", 5MB

	The all log levels logged. You can set the value of LOG_LEVEL_ERROR for redice the logger work.
	rart_cat is a category used by the application and logged to the /ERMO/log/tech/rart.log with the rotation for 5MB size of the file

4. Utilisation de l'application
	4.1 Installation 
		 dpkg --install --instdir=/ERMO ramsesRouter_64_1.0.0.deb 
	4.2 Démarrage dans un console via ligne de commande 
	4.3 Utilisation est possible comme un demone , raRt --daemon .
	4.4 Exemple de la ligne de commande : ./raRt --config /ERMO/param --log /ERMO/log/tech 
		--config le répértoirou se trouve le fichier de configuraiton
		--log le répértoire du log extra
		
5. Maintenace
	5.1 /etc/log/syslog est utilisé pour signalé differents message de l'application vers le stdout  et le fichier log voir 4.4
	5.2 Fichier de log par defaut /ERMO/log/tech/raRt.log , peut être utilser pour logger les informaitons suplimentaires. Voir 3.2
6. Installation de boost 
	6.1 Ajouter dans /etc/apt/sources.list deb http://backports.debian.org/debian-backports lenny-backports main 
	En cas de problème des mirroirs il faut ajouter 
	deb http://ftp.fr.debian.org/debian/ lenny main
	deb-src http://ftp.fr.debian.org/debian/ lenny main
	1.2  apt-get -t lenny-backports install libboost1.40-all-dev

