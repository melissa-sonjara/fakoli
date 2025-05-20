<?php
/**\addtogroup login
 * @{
 */


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


/**************************************************************
 * 
 * Title: login_token.inc
 * 
 * Description: datamodel classes for login token.
 * 
 * author Janice Gallant for Sonjara, Inc.
 * 
 * date: 4/19/10
 *
 ****************************************************************/

/* Installation Note:
   The user datamodel must have the following relation:
 
function __construct()
{
	$token = querySingle(LoginToken, "WHERE user_id={$this->user_id}");
	return $token;
}

*/

/*
 * Provide temporary token logins to teachers who
 * have forgotten their passwords.
 */
class LoginToken extends DataItem
{
	var $fields = array(
				"token_id"			=>	'Number',
				"token"				=>	'String',
				"expire_date"		=>	'Date',
				"user_id"			=>	'Number'
	);
	
	var $relations = array(
					"SiteUser"		=>	""
	);
	
	
	function SiteUser()
	{
		$mgr = new UserManager();
		return $mgr->getUser($this->user_id);
	}
	
	/*
	 * generateToken
	 *
	 * Create a password login token using upper and lower case charaters
	 * and numbers. Length is 7.
	 */
	static function generateToken($user_id)
	{
		$chars .= 'BDGHJLMNPQRSTVWXZ' . "AEUY" . '23456789' .'aeuy' . 'bdghjmnpqrstvz';
		$length = 7;
		
		$code = '';
		$alt = time() % 2;
		for ($i = 0; $i < $length; $i++) 
		{
			$code .= $chars[(rand() % strlen($chars))];
			$alt = 0;
		}
			
	   	if($code)
    	{    	
    		$token = new LoginToken();
  			$token->token = $code;
			$expires = mktime(0, 0, 0, date("m"), date("d")+2, date("y"));
			$token->expire_date = date('Y-m-d', $expires);
			$token->user_id = $user_id;
			$token->save();
    	}
        	
    	return $token;
	}
	
	
	function __construct()
	{
		$this->table = "login_token";
		$this->primary_key = "token_id";
					
		// Patch in the user class, since this can be overridden by the application
		$mgr = new UserManager();
		$this->relations["SiteUser"] = $mgr->getUserClass(); 
		
		parent::__construct(func_get_args());	
	}
}

/**@}*/?>
