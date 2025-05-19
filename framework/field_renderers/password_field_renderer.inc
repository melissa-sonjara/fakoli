<?php
/**\addtogroup fieldrenderers
 * @{
 */

/**************************************************************

 Copyright (c) 2007-2010 Sonjara, Inc

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
	var $encryptor;	//< Custom encryptor
	var $forceReset = false; ///< true if user logged in with a temporary token
	var $numeric = false; //< numeric code (PIN)
	var $allowEmpty = false;
	
	function __construct(&$parent)
	{
		parent::__construct($parent);
		
	}
	
	function addValidatorsToForm($field, $required = false)
	{
		$label = str_replace("*", "", $this->parent->prettifyFieldName($field));
		$this->parent->match($field, $label, "{$field}_confirm", "Confirmation");
		$this->parent->password($field, $label);	
	}
	
	function renderScript($field)
	{
		if (!$this->_includedPasswordScript)
		{
?>
<script type='text/javascript'>
<!--
function <?echo $this->parent->id?>_<?echo $field?>_showPassword()
{
	var link = document.getElementById("<?echo $this->parent->id?>_<?echo $field?>_link");
	var block = document.getElementById("<?echo $this->parent->id?>_<?echo $field?>_block");
	document.id("<?echo $this->parent->id ?>_<?echo $field?>_reset_password").value = 1;
	
	link.style.display = "none";
	block.style.display = "block";
	ModalDialog.recenterActiveDialog();
}
//-->
</script>
<?			$this->_includedPasswordScript = true;
		}		
	}
	
	function renderPasswordBlock($field, $display = "block")
	{
		$enter = $this->parent->passwordEnterLabel;
		$confirm = $this->parent->passwordConfirmLabel;
		$size = $this->numeric ? "6" : "20";
		$onKeyPress = $this->numeric ? " onkeypress='return maskInput(event, $allowneg);'" : ""
?>
			 <table border="0" cellspacing="0" cellpadding="0" id="<?echo $this->parent->id ?>_<?echo $field?>_block" style="padding: 0px; display: <?echo $display ?>">
			  <tr>
  				<td style="vertical-align: top; text-align: left"><?echo $enter?>&nbsp;</td>
  				<td style="padding-bottom: 2px"><input type="password" autocomplete="new-password" name="<?echo $field?>" id="<?echo $this->parent->id?>_<?echo $field?>" size="<?echo $size?>"<?echo $onKeyPress?> value=""/></td>
 			  </tr>
 			  <tr>
  				<td style="vertical-align: top; text-align: left"><?echo $confirm?>&nbsp;</td>
  				<td><input type="password" name="<?echo $field?>_confirm" id="<?echo $this->parent->id?>_<?echo $field?>_confirm" size="<?echo $size?>"<?echo $onKeyPress?> value=""/></td>
 			  </tr>
 			 </table>		
<?
	}
	
	function renderField($field)
	{
		$pk = $this->parent->data->getPrimaryKey();
	
		$this->_startField($field);
		
		if (($this->parent->data->get($pk) && !$this->forceReset) || $this->allowEmpty)
		{
			$verb = ($this->allowEmpty && !$this->parent->data->get($pk)) ? "set" : "reset";
			
			echo "<a id='{$this->parent->id}_{$field}_link' href='#' onclick='{$this->parent->id}_{$field}_showPassword();return false;'>Click to {$verb} password</a>\n";
			$this->renderPasswordBlock($field, "none");
			echo "<input type='hidden' id='{$this->parent->id}_{$field}_reset_password' name='{$this->parent->id}_{$field}_reset_password' value=''/>\n";
		}
		else
		{
			$this->renderPasswordBlock($field);
			echo "<input type='hidden' id='{$this->parent->id}_{$field}_reset_password' name='{$this->parent->id}_{$field}_reset_password' value='1'/>\n";
		}
		
		$this->_endField($field);
		
	}
	
	function renderReadOnly($field)
	{
		$this->_startField($field);

		echo "<i>Password is hidden</i>\n";
		
		$this->_endField($field);
	}
	
	function preProcess($field = "")
	{
		$pk = $this->parent->data->getPrimaryKey();
		
		if (!$this->parent->data->$pk || $_POST["{$this->parent->id}_{$field}_reset_password"])
		{
			$password = $_POST[$field];
		}
		else
		{
			if ($this->parent->data->getFilter() == null)
			{
				$this->parent->data->setFilter(new ExclusionFilter());
			}
			
			$filter = $this->parent->data->getFilter();
			
			if ($filter->type == "ExclusionFilter")
			{
				$filter->add($field);
			}
			else
			{
				$filter->remove($field);
			}
		}
		
		if ($this->allowEmpty && !$password)
		{
			$this->parent->data->set($field, "");
		}
		else
		{
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
			
			$this->parent->data->set($field, $password);
		}
	}	
}

/**@}*/?>