<?php

/**************************************************************

 Copyright (c) 2007,2008 Sonjara, Inc

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

require_once realpath(dirname(__FILE__))."/../field_renderers.inc";

/**
 * Field renderer for password data fields. Renders as a compound
 * control with password and confirmation fields. Additionally,
 * if updating an existing record the user must click "Update Password"
 * to gain access to these fields.
 */
class PasswordFieldRenderer extends FieldRenderer
{
	var $encryptor;
	
	function PasswordFieldRenderer(&$parent)
	{
		$this->FieldRenderer($parent);
		
		$this->parent->match("{$this->parent->id}_password", "Password", "{$this->parent->id}_confirm", "Confirmation");
		$this->parent->password("{$this->parent->id}_password", "Password");
	}
	
	function renderScript($field)
	{
		if (!$this->_includedPasswordScript)
		{
?>
<script type='text/javascript'>
<!--
function <?echo $this->parent->id?>_showPassword()
{
	var link = document.getElementById("<?echo $this->parent->id?>_link");
	var block = document.getElementById("<?echo $this->parent->id?>_password_block");
	document.forms['<?echo $this->parent->id ?>'].<?echo $this->parent->id ?>_reset_password.value = 1;
	
	link.style.display = "none";
	block.style.display = "block";
}
//-->
</script>
<?			$this->_includedPasswordScript = true;
		}		
	}
	
	function renderPasswordBlock($field, $display = "block")
	{
?>
			 <input type="hidden" name="<?echo $field ?>" value="">
			 <table border="0" cellspacing="0" cellpadding="0" id="<?echo $this->parent->id ?>_password_block" style="padding: 0px; display: <?echo $display ?>">
			  <tr>
  				<td style="vertical-align: top; text-align: left">Enter&nbsp;</td>
  				<td style="padding-bottom: 2px"><input type="password" autocomplete="off" name="<?echo $this->parent->id?>_password" size="20" value=""></td>
 			  </tr>
 			  <tr>
  				<td style="vertical-align: top; text-align: left">Confirm&nbsp;</td>
  				<td><input type="password" name="<?echo $this->parent->id?>_confirm" size="20" value=""></td>
 			  </tr>
 			 </table>		
<?
	}
	
	function renderField($field)
	{
		$pk = $this->parent->data->primary_key;
	
		$this->_startField($field);
		
		if ($this->parent->data->$pk)
		{
			echo "<a id='{$this->parent->id}_link' href='#' onclick='{$this->parent->id}_showPassword();return false;'>Click to reset password</a>\n";
			$this->renderPasswordBlock($field, "none");
			echo "<input type='hidden' name='{$this->parent->id}_reset_password' value=''>\n";
		}
		else
		{
			$this->renderPasswordBlock($field);
		}
		
		$this->_endField($field);
		
	}
	
	function renderReadOnly($field)
	{
		echo "<tr>\n";
		$this->_printLabel($field);
		echo "<td";
		if ($this->parent->valueCSS) echo " class='{$this->parent->valueCSS}'";
		echo "><i>Password is hidden</i></td>\n";
		echo "</tr>\n";
	}
	
	function preProcess($field = "")
	{
		$pk = $this->parent->data->primary_key;
		
		if (!$this->parent->data->$pk || $_POST["{$this->parent->id}_reset_password"])
		{
			$password = $_POST["{$this->parent->id}_password"];
		}
		else
		{
			if ($this->parent->data->filter == null)
			{
				$this->parent->data->filter = new ExclusionFilter();
			}
			
			if ($this->parent->data->filter->type == "ExclusionFilter")
			{
				$this->parent->data->filter->add($field);
			}
			else
			{
				$this->parent->data->filter->remove($field);
			}
		}
		
		$encryptor = $this->parent->passwordEncryptor;
		
		trace("Encryptor: $encryptor", 3);
		
		if ($encryptor)
		{
			$password = $this->parent->data->$encryptor($password);
		}
		else
		{
			$password = crypt($password);
		}
		
		$this->parent->data->$field = $password;
	}	
}

?>