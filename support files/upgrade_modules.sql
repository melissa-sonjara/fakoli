ALTER TABLE `module`
 ADD COLUMN `menu_id` INTEGER UNSIGNED NOT NULL DEFAULT 0 AFTER `css_class`,
 ADD COLUMN `menu_parameters` VARCHAR(200) AFTER `menu_id`,
 ADD COLUMN `global` TINYINT UNSIGNED NOT NULL DEFAULT 0 AFTER `menu_parameters`,
 ADD COLUMN `global_position` VARCHAR(50) AFTER `global`;
