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

require_once realpath(dirname(__FILE__)."/../../framework/data_item.inc");

$siteUserRoles = array(	"admin"		=>	"Administrator",
						"member"	=>	"Site Member");

class SiteRole extends DataItem
{
	// Fields
	
	var $fields = array("role_id"		=>	Number,
						"role"			=>	String,
						"name"			=>	String,
						"description"	=>	Text);
	
	
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
		$roles = query(SiteRole, "WHERE role!='super' ORDER BY name");
		
		$rarr = array();
		
		foreach($roles as $role)
		{
			$rarr[$role->role] = $role->name;
		}
		
		return $rarr;
	}
}

	
/**
 * This class maps the site_user table.
 */
class SiteUser extends DataItem
{
	// Fields
	
	var $fields = array("user_id"			=>	Number,
						"first_name"		=>	String,
						"last_name"			=>	String,
						"role"				=>	String,
						"title"				=>	String,
						"username"			=>	String,
						"password"			=>	Password,
						"email"				=>	String,
						"active"			=>	Boolean,
						"notes"				=>	Text,
						"composite_class"	=>	String);
						
	// Relations

	function SiteUser()
	{
		$this->primary_key = "user_id";
		$this->table = "site_user";
		
		$this->default_format = "{first_name} {last_name}";
		
		$this->DataItem(func_get_args());
	}
	
	function getAdminForm()
	{		
		return "user_form.php";
	}
}

?>