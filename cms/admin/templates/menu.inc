
<?php
	if ($admin_menu)
	{
		if (!$menu_item) $menu_item = basename($_SERVER['PHP_SELF']);
?>
<div id="menu">
<?
		$menuSectionIdx = 0;
		
		foreach($admin_menu as $section => $items)
		{
?>
 <h4 class="toggler atStart"><?echo $section ?></h4>
 <div class="atStart">
<?			
			foreach($items as $item => $properties)
			{
				if ($item == $menu_item) 
				{
					$class = 'selectedadminitem';
					$selectedMenuSectionIdx = $menuSectionIdx;
				}
				else
				{
					$class = 'adminmenuitem';
				}
				if (checkRole($properties["role"]))
				{
					echo "<a class='$class' href='{$properties['page']}{$menu_params}'>{$item}</a>\n";
				}
			}
			
			++$menuSectionIdx;
?>
</div>
<?
		}
?>
</div>
<?
if (!$selectedMenuSectionIdx) $selectedMenuSectionIdx = 0;

?>
<script type="text/javascript">
var accordionOpenSection = <?echo $selectedMenuSectionIdx?>;
</script>
<?
	}
?>