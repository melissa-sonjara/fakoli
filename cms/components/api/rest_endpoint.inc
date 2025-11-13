<?php
Fakoli::using("api");
Fakoli::usingFeature("api_helper");

class RESTEndPoint extends APIEndPoint
{
    protected $helper;
    protected $roles;
    
    function __construct($class, $roles = "", $searchFilter = null, $outputFilter = null, $constraints = "")
    {
        $this->roles = $roles;
        $this->helper = new APIHelper($class, $searchFilter, $outputFilter, $constraints);
    }
    
    function checkAccess()
	{
		return checkRole($this->roles);
	}
	
	function allowAnonymous()
	{
		return ($this->roles == "");
	}

    function query()
    {
        return $this->helper->query();
    }
    
    function get($id, $relationships = "")
    {
        return $this->helper->get($id, $relationships);
    }
    
    function post()
    {
        return $this->helper->post();
    }
    
    function put()
    {
        return $this->helper->put();
    }

    function dispatch()
    {
        global $method;
        if ($method == "GET")
		{
			if (isset($_REQUEST["id"]))
			{
				$this->get($_REQUEST["id"], $_REQUEST["relations"]);
				return;
			}

			return $this->query();
		}

		$m = strtolower($method);
		$this->$m();
    }
}