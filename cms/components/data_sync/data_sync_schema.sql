-- Component Schema for data_sync
--
-- Author: Andy Green
--
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx

-- START Version 1.0

CREATE TABLE `data_import_field_mapping` (
	`mapping_id` int(11) NOT NULL AUTO_INCREMENT,
	`class` VARCHAR(255) NOT NULL,
	`client_field` VARCHAR(255) NOT NULL,
	`import_column` VARCHAR(255) NULL,
	`matching` TINYINT(3) UNSIGNED NOT NULL DEFAULT 0,
	PRIMARY KEY (`mapping_id`)
) ENGINE=InnoDB;

-- END Version 1.0

-- START Version 1.1

ALTER TABLE `data_import_field_mapping` ADD COLUMN `required` TINYINT(3) UNSIGNED NOT NULL DEFAULT 0;

-- END Version 1.1