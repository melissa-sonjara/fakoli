<?php
Fakoli::usingFeature("api_helper");

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
	
	function ApiManager()
	{

	}
	
	static function registerEndPoint($endpoint, $handler)
	{
		APIManager::$endpoints[$endpoint] = $handler;
	}

	static function getEndPoint($endpoint)
	{
		if (!APIManager::$endpoints)
		{
			APIManager::$endpoints = array();
			ComponentManager::fireEvent('RegisterAPIEndPoints');
		}
		
		return APIManager::$endpoints[$endpoint];
	}
	
	static function dispatch($endpoint)
	{
		global $method;
		global $user;
		
		$handler = APIManager::getEndPoint($endpoint);
		if (!$handler)
		{
			throw new FakoliException("API Endpoint not found");
		}
		
		$m = strtolower($method);
		if (!method_exists($handler, $m))
		{
			throw new FakoliException("Unsupported HTTP Verb");
		}
		
		if ($handler->allowAnonymous())
		{
			$handler->$m();
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
		
		$handler->$m();
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
			
			LoginManager::recordLoginAttempt($username, "api", null, "success");
			Fakoli::JSONReturn($token, true);
		}
		else
		{
			return "ERROR";
		}
	}
	
	static function registerAPIEndPoints()
	{
		APIManager::registerEndPoint("authenticate", new AuthenticateAPIEndPoint());
	}
	
	static function setDefaults()
	{
		//TODO: Set default configuration parameters here
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

?>