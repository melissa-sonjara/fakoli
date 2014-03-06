<?
/**************************************************************

 Copyright (c) 2010 Sonjara, Inc

 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:

 The above copyright notice and this permission notice shall be
 included in all copies or substantial portions of the Software.

 Except as contained in this notice, the name(s) of the above 
 copyright holders shall not be used in advertising or otherwise 
 to promote the sale, use or other dealings in this Software 
 without prior written authorization.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.

*****************************************************************/

$siteUserRoles = array(	"admin"		=>	"Administrator",
						"member"	=>	"Site Member");

class SiteRole extends DataItem
{
	// Fields
	
	var $fields = array("role_id"		=>	Number,
						"role"			=>	String,
						"name"			=>	String,
						"description"	=>	Text,
						"priority"		=>	Number,
						"home_page"		=>	String);
	
	
	// Relations
	var $relations = array("Site"	=>	Site);
	
	function Site()
	{
		return $this->getRelated(Site);
	}
	
	function SiteRole()
	{
		$this->table = "site_role";
		$this->primary_key = "role_id";
		$this->default_format = "{name}";
		
		$this->DataItem(func_get_args());
	}

	/**
	 * Retrieves the array of roles and their names for the current site for use in a CMS form.
	 * @return array
	 */
	static function getRolesArray()
	{
		$roles = query(SiteRole, "WHERE role!='super' ORDER BY priority, name");
		
		$rarr = array();
		
		foreach($roles as $role)
		{
			$rarr[$role->role] = $role->name;
		}
		
		return $rarr;
	}
	
	// Given a role code, return the record
	static function getRole($role)
	{
		$names = query(SiteRole, "WHERE role='$role'");
		if(count($names) > 0)
			return $names[0];
	}
	
	static function getRoleLookup()
	{
		return IndexedQuery::create(SiteRole, "ORDER by priority, name", "role")->execute();
	}
	
	static function getHomePage()
	{
		$roles = SiteRole::getRoleLookup();
		foreach($roles as $role => $details)
		{
			if (checkRole($role)) return $details->home_page;
		}
		
		return "";
	}
	
	/*
	 * Given a comma delimited list of roles (as role is stored
	 * in the user role field, return the list of corresponding
	 * role names.
	 */
	static function getRoleNames($role)
	{
		$roles = explode(",", $role);
		if(count($roles) > 0)
		{
			foreach($roles as $role)
			{
				$name = SiteRole::getRole($role);
				if($name)
					$names[] = $name->name;
			}
		}
	
		return implode(", ", array_values($names));
	}
}

?>