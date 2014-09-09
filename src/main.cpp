#include "common.h"
#include "udpRelayServer.h"
#include "tcpRelayServer.h"
#include "paramsManager.h"


#ifndef NDEBUG
boost::logging::logger lgr;
#endif

#define ERR_BUF_SIZE 1024

boost::asio::io_service g_io_service;
boost::logging::sink* 	g_psinkfile;
rart::ParamsManagerPtr 	g_spParamsManager;

char g_szcConfigPath[256];
char g_szcLogPath[256];
int g_nLogLevel = 0;

char g_szcLogConfigPath[256];

bool g_asDaemon = false;
#ifdef BOOST_NO_CODE_GENERATION_FOR_LOG
//MTKMain glblMTKInst;
#endif
char g_szcVerson[] ="1.2.1";

#ifndef  WIN32

void terminate(int signal_number)
{
	
	{
		//ecriture de log d'arrêt
		std::string sStartLogPath(g_szcLogPath);
		sStartLogPath += "/";
		sStartLogPath += "raRt_events.log";
		std::ofstream of(sStartLogPath.c_str(), std::ios::out | std::ios_base::app);
		boost::posix_time::ptime t(boost::posix_time::microsec_clock::local_time()); 	
		of << "raRt arrêt : " << t << std::endl;
		of.close();
	}
	g_io_service.stop();
	syslog(LOG_INFO, "raRt stoped by %d signal.\n", signal_number); 
	closelog();
	
}

void reload_config(int signal_number)
{
	int nOldLogLevel = g_nLogLevel; 
	rart::ParamsManager paramsManager(g_szcConfigPath);
	int nRes = paramsManager.load();	
	log_reload(g_szcLogConfigPath);
}

void set_signals()
{
	signal(SIGHUP, terminate);
	signal(SIGINT, terminate);
	signal(SIGQUIT, terminate);
	signal(SIGKILL, terminate);
	signal(SIGTERM, terminate);
#ifndef DEBUG
	signal(SIGTRAP, terminate);
#endif
	signal(SIGABRT, terminate);
#ifdef SIGEMT
	signal (SIGEMT, terminate);
#endif
	signal(SIGFPE, SIG_IGN);
	signal(SIGBUS, SIG_IGN);
	signal(SIGSEGV, SIG_DFL);
	signal(SIGSYS, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGALRM, SIG_IGN);
	signal(SIGXCPU, SIG_IGN);
	signal(SIGXFSZ, SIG_IGN);
	signal(SIGVTALRM, SIG_IGN);
	signal(SIGPROF, SIG_IGN);
	signal(SIGUSR1, reload_config);
	signal(SIGUSR2, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
}

void make_daemon()
{
	pid_t pid;
	int fd;
	set_signals();
	if (getppid() != 1)
	{
		if ((pid = fork()) < 0)
		{
			char err_descr[ERR_BUF_SIZE];
			if (strerror_r(errno, err_descr, sizeof(err_descr)) == 0)
			{
				syslog(LOG_ERR, "fork() 1 error: %s\n", err_descr);
			}
			exit(1);
		}
		else if (pid > 0)
		{
			exit(0);
		}
		// modify your mask
		umask(0);

		if (setsid() < 0)
		{
			char err_descr[ERR_BUF_SIZE];
			if (strerror_r(errno, err_descr, sizeof(err_descr)) == 0)
			{
				syslog(LOG_ERR, "setsid() error: %s\n", err_descr);
			}
		}
		if ((pid = fork()) < 0)
		{
			char err_descr[ERR_BUF_SIZE];
			if (strerror_r(errno, err_descr, sizeof(err_descr)) == 0)
			{
				syslog(LOG_ERR, "fork() 2 error: %s\n", err_descr);
			}
			exit(1);
		}
		else if (pid > 0)
		{
			exit(0);
		}
	}
	for (fd = 0; fd < sysconf(_SC_OPEN_MAX); fd++)
	{
		close(fd);
	}
	errno = 0;
	chdir("/");
	
	/* Close out the standard file descriptors */
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    log_init(g_szcLogConfigPath);
	g_asDaemon = true;
}

#endif

int run()
{
	int nRes = 0;
	try
	{
		log_notice(rart_category, "raRt service starting ...");
		rart::UdpRelayServer udpServer(g_io_service, g_spParamsManager);
		rart::SessionManagerPtr spSessionManager =
				udpServer.getSessionManager();

		log_notice(rart_category, "raRt service started.");
		//
		udpServer.start_receive();
		//
		boost::shared_ptr<boost::thread> thread(new boost::thread(boost::bind(&boost::asio::io_service::run, &g_io_service)));
		//
		thread->join();
		//
		log_notice(rart_category, "raRt service stoped.");

	} catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		log_notice(rart_category, "raRt service get an exception : %s .", e.what());
		nRes = -1;
	}
	return nRes;

}

void usage()
{
	fprintf(stderr, "usage: kaRt [options]\n"
		"options:\n"
		"   --daemon         - daemon mode\n"
		"   --verbose        - verbose\n"
		"   --config         - path to raRt.cfg\n"
		"   --log			 - path to log directory\n"
		"   --version		 - version of application\n");
}

int main(int argc, char *argv[])
{
	int nRes = 0;
	bool bMakeDaemon = false;
	memset(g_szcConfigPath, 0, sizeof(g_szcConfigPath));
	memset(g_szcLogPath, 0, sizeof(g_szcLogPath));
	memset(g_szcLogConfigPath, 0, sizeof(g_szcLogConfigPath));
	//default paths configuration
	strcpy(g_szcConfigPath, "/ERMO/param");
	strcpy(g_szcLogPath, "/ERMO/log/tech");
	strcpy(g_szcLogConfigPath, "/ERMO/param");
	

	try
	{
		boost::program_options::options_description desc(
				"raRt command line usage");

		desc.add_options()("daemon", "deamon mode")("verbose",
				boost::program_options::value<int>(), "verbose level of log")(
				"config", boost::program_options::value<std::string>(),
				"directory where reside raRt.cfg file")("log",
				boost::program_options::value<std::string>(),
				"log directory where application log extra data")("help",
				"help")("version", "version de l'applicaiton");
		boost::program_options::variables_map vm;
		boost::program_options::store(
				boost::program_options::parse_command_line(argc, argv, desc),
				vm);
		boost::program_options::notify(vm);

		if (vm.count("daemon"))
		{
			bMakeDaemon = true;
		}
		if (vm.count("verbose"))
		{
			g_nLogLevel = vm["verbose"].as<int> ();
		}
		if (vm.count("config"))
		{
			strcpy(g_szcConfigPath, vm["config"].as<std::string> ().c_str());
			strcpy(g_szcLogConfigPath, vm["config"].as<std::string> ().c_str());
			
		}

		if (vm.count("log"))
		{
			strcpy(g_szcLogPath, vm["log"].as<std::string> ().c_str());
		}

		if (vm.count("help"))
		{
			std::cout << desc << std::endl;
			exit(0);
		}
		
		if(vm.count("version"))
		{
			std::cout << "Rart version : " << g_szcVerson << std::endl;
			exit(0);	
		}
	} catch (std::exception& e)
	{
		std::cout << "Command line parsing exception :  " << e.what()
				<< std::endl;
		exit(1);
	}
	{
		//ecriture de log de démarrage
		std::string sStartLogPath(g_szcLogPath);
		sStartLogPath += "/";
		sStartLogPath += "raRt_events.log";
		std::ofstream of(sStartLogPath.c_str(), std::ios::out | std::ios_base::app);
		boost::posix_time::ptime t(boost::posix_time::microsec_clock::local_time()); 	
		of << "raRt démarrage : " << t << " version : " << g_szcVerson << std::endl;
	}

#ifndef WIN32
	openlog("raRt", LOG_PID | LOG_CONS, LOG_USER);
#endif
	// init params handler 
	g_spParamsManager.reset(new rart::ParamsManager(
				g_szcConfigPath));

	nRes = g_spParamsManager->load();
	//
	strcat(g_szcLogConfigPath, "/rart_log.conf");
	//
	if (nRes == 0)
	{
		if (true == bMakeDaemon)
		{
			make_daemon();
		}
		else
		{
			log_init(g_szcLogConfigPath);
		}
		//start application service
		run();		
	}
	else
	{
#ifndef WIN32
		syslog(LOG_ERR, "can't load parameters, bye\n");
#endif 
	}
	g_spParamsManager.reset();
	
#ifndef WIN32
	closelog();
#endif
	

}
