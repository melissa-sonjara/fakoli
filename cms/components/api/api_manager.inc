<?php
/**\addtogroup api
 * @{
 */


Fakoli::usingFeature("api_helper");
Fakoli::using("section");

class APIEndPoint
{
	function __construct()
	{
	}
	
	function checkAccess()
	{
		return false;
	}
	
	function allowAnonymous()
	{
		return false;
	}

	function dispatch()
	{
	}
}

class SimpleQueryAPIEndPoint extends APIEndPoint
{
	protected $helper;
	protected $roles;
	
	function __construct($class, $roles = "", $searchFilter = null, $outputFilter = null, $constraints = "")
	{
		$this->roles = $roles;
		$this->helper = new APIHelper($class, $searchFilter, $outputFilter, $constraints);
	}
	
	function get()
	{
		return $this->helper->query();
	}
	
	
	function checkAccess()
	{
		return checkRole($this->roles);
	}
	
	function allowAnonymous()
	{
		return ($this->roles == "");
	}

	function dispatch()
	{
		global $method;
		if ($method == "GET") return $this->get();
		throw new FakoliException("Unsupported HTTP Verb");
	}
}

class AuthenticateAPIEndPoint extends APIEndPoint
{
	function get()
	{
		Fakoli::JSONreturn(APIManager::validateAPILogin());
	}
	
	function post()
	{
		Fakoli::JSONreturn(APIManager::validateAPILogin());
	}
	
	function allowAnonymous()
	{
		return true;
	}

	function dispatch()
	{
		global $method;
		if ($method == "GET") return $this->get();
		if ($method == "POST") return $this->post();
		throw new FakoliException("Unsupported HTTP Verb");
	}
}

class ExtendTokenAPIEndPoint extends APIEndPoint
{
	function get()
	{
		Fakoli::JSONreturn(APIManager::extendToken());
	}
	
	function post()
	{
		Fakoli::JSONreturn(APIManager::extendToken());
	}
	
	function allowAnonymous()
	{
		return true;
	}

	function dispatch()
	{
		global $method;
		if ($method == "GET") return $this->get();
		if ($method == "POST") return $this->post();
		throw new FakoliException("Unsupported HTTP Verb");
	}
}


/**
 *
 * Provides a central management class for event handlers and 
 * common functionality for the api component.
 */
class APIContentManager implements ISectionContentManager
{
	function getDefaultPage($section) { return $section->default_page; }
	
	function getContent($section, $identifier)
	{
		// Create Mock SectionContent object
		$content = new SectionContent();
		$content->section_id = $section->section_id;
		$content->identifier = $identifier;
		return $content;
	}
	
	function sendContent($section, $identifier)
	{
		if (!$identifier)
		{
			throw new FakoliException("Missing identifier");
		}
		
		APIManager::dispatch($identifier);
	}
	
	function getAdminView($section)
	{
		return new APIContentAdminView($section);
	}
}

class APIContentAdminView extends AbstractSectionContentAdminView
{
	function __construct($section)
	{
		parent::__construct($section);
	}
	
	function handlePOST()
	{
	}
	
	function drawView()
	{
		global $script;
		
		
		/*
		$tokens = Query::create(ShareToken, "ORDER BY created_date")->execute();
		$table = new DataListView($tokens, "share_tokens");
		$table->column("Token", "<a href=''>{token}</a>", true)
		->column("Type", "{item_type}", true)
		->column("Shared By", "{User.getFullName()}", true)
		->column("Active", "<i class='fa-fw fas fa-{active:check/times}'></i>", true, "text-align: center")
		->column("# Accesses", "{countAccesses()}", true, "text-align: right");
		
		$table->pageSize = 20;
		$table->filter = true;
		$table->sortable = true;
		$table->emptyMessage = "No items have been shared";
		$table->excelFile = "share_tokens.xls";
		
		$script .= $table->writeScript();
		
		echo "<h3>Share Tokens</h3>";

		$table->drawView();
		*/
	}
}
class APIManager
{
	private static $endpoints = null;
	private static $endpointVerbs = null;

	function ApiManager()
	{

	}
	
	static function registerEndPoint($endpoint, $handler, $verbs = "GET")
	{
		APIManager::$endpoints[$endpoint] = $handler;
		$verbs = explode(",", strtoupper($verbs));
		APIManager::$endpointVerbs[$endpoint] = array_combine($verbs, $verbs);
	}

	static function getEndPoint($endpoint)
	{
		if (!APIManager::$endpoints)
		{
			APIManager::$endpoints = array();
			APIManager::$endpointVerbs = array();
			ComponentManager::fireEvent('RegisterAPIEndPoints');
		}
		
		return APIManager::$endpoints[$endpoint];
	}
	
	static function verbSupported($endpoint, $verb)
	{
		$verb = strtoupper($verb);
		return array_key_exists($verb, APIManager::$endpointVerbs[$endpoint]);
	}

	static function dispatch($endpoint)
	{
		global $method;
		global $user;
		
		trace("APIManager::dispatch endpoint $endpoint method $method", 3);

		$handler = APIManager::getEndPoint($endpoint);
		if (!$handler)
		{
			throw new FakoliException("API Endpoint not found");
		}
		
		
		if (!APIManager::verbSupported($endpoint, $method))
		{
			throw new FakoliException("Unsupported HTTP Verb");
		}
		
		if ($handler->allowAnonymous())
		{
			$handler->dispatch();
			return;
		}
		
		$t = checkIdentifier($_GET["token"]);
		
		if (!$t)
		{
			throw new FakoliException("No Authentication Token Provided");
		}
		
		$token = APIToken::getToken($t);
		if (!$token)
		{
			throw new FakoliException("Authentication Token Not Found");
		}
		
		if (!$token->active)
		{
			throw new FakoliException("Invalid Token");
		}
		
		$user = $token->User();
		
		if (!$handler->checkAccess())
		{
			throw new FakoliException("Permission Denied");
		}
		
		$handler->dispatch();
	}
	
	function validateAPILogin()
	{
		global $user;
		
		$mgr = new UserManager();
		
		$username = $_REQUEST[$mgr->getUsernameField()];
		$password = $_REQUEST["password"];
		
		if (Settings::getValue("login", "reject_all_blank_passwords"))
		{
			// JDG - guard against data not found exception
			if(!$username OR !$password)
			{
				LoginManager::recordLoginAttempt($username, "api", null, "failure");
				LoginManager::$error = "Incorrect user name or password.";
				return "ERROR|".LoginManager::$error;
			}
		}
		else
		{
			if(!$username)
			{
				LoginManager::recordLoginAttempt($username, "api", null, "failure");
				LoginManager::$error = "Incorrect user name or password.";
				return "ERROR|".LoginManager::$error;
			}
		}
		
		ComponentManager::fireEvent("OnLoginSubmit");
		
		$user = $mgr->validatePassword($username, $password);
		if ($user)
		{
			$token = APIToken::getUserToken($user->user_id);
			if (!$token)
			{
				$token = new APIToken();
				$token->token = plainGUID();
				$token->user_id = $user->user_id;
				$token->created_date = now();
				$token->expiry_date = null;
				$token->active = true;
				$token->last_access = now();
				$token->save();
			}
			
			$tokenLifetime = Settings::getValue("api", "token_lifetime");
			if ($tokenLifetime)
			{
				$expiryDate = new DateTime();
				$expiryDate->modify("+".$tokenLifetime);
				$token->expiry_date = $expiryDate->format("Y-m-d H:i:s T");
			}
			
			LoginManager::recordLoginAttempt($username, "api", null, "success");
			Fakoli::JSONReturn($token, true);
		}
		else
		{
			return "ERROR";
		}
	}
	
	function extendToken()
	{
		$t = checkIdentifier($_GET["token"]);
		
		$token = APIToken::getToken($t);
		
		$tokenLifetime = Settings::getValue("api", "token_lifetime");
		if ($tokenLifetime)
		{
			$expiryDate = new DateTime();
			$expiryDate->modify("+".$tokenLifetime);
			$token->expiry_date = $expiryDate->format("Y-m-d H:i:s T");
			$token->save();
		}
	}
	
	static function registerAPIEndPoints()
	{
		APIManager::registerEndPoint("authenticate", new AuthenticateAPIEndPoint());
		APIManager::registerEndPoint("extend", new AuthenticateAPIEndPoint());
	}
	
	static function setDefaults()
	{
		Settings::setDefaultValue("api", "token_lifetime", "30 days", "String",  "Specifies how long API authorization tokens stay valid. Leave blank to have tokens never expire");
	}

	static function registerSectionContentManager()
	{
		SectionContentManagerMap::registerManager("API", new APIContentManager());
	}
	
	static function upgradeComponent($version)
	{
		$mgr = new APIUpgradeManager();
		$mgr->upgrade($version);
	}
}

/**@}*/?>