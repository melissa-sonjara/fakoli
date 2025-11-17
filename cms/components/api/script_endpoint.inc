<?php
Fakoli::using("api");
Fakoli::usingFeature("api_helper");

class ScriptEndPoint extends APIEndPoint
{
    protected $component;
    protected $script;
    protected $roles;
    
    function __construct($component, $script, $roles = "")
    {
        $this->component = $component;
        $this->script = $script;
        $this->roles = $roles;

        parent::__construct();
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
        return $this->dispatch();
    }
    
    function get($id, $relationships = "")
    {
        return $this->dispatch();
    }
    
    function post()
    {
        return $this->dispatch(); 
    }
    
    function put()
    {
        return $this->dispatch();
    }

    function dispatch()
    {
        echo Fakoli::evaluateFile("{$this->component},{$this->script}");
    }
}