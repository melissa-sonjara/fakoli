-- Component Schema for data_sync
--
-- Author: Andy Green
--
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx

-- START Version 1.0

CREATE TABLE data_import_field_mapping (
	mapping_id int(11) NOT NULL AUTO_INCREMENT,
	class VARCHAR(255) NOT NULL,
	client_field VARCHAR(255) NOT NULL,
	import_column VARCHAR(255) NULL,
	matching TINYINT(3) UNSIGNED NOT NULL DEFAULT 0,
	PRIMARY KEY (mapping_id)
) ENGINE=InnoDB;

-- END Version 1.0

-- START Version 1.1

ALTER TABLE data_import_field_mapping ADD COLUMN required TINYINT(3) UNSIGNED NOT NULL DEFAULT 0;

-- END Version 1.1

-- START Version 1.2

ALTER TABLE data_import_field_mapping ADD COLUMN `grouping` TINYINT(3) UNSIGNED NOT NULL DEFAULT 0;

-- END Version 1.2

-- START Version 1.3

ALTER TABLE data_import_field_mapping ADD COLUMN import_profile_id int(10) NOT NULL DEFAULT 0;

CREATE TABLE data_import_profile
(
	import_profile_id	int(10) not null auto_increment,
	profile				varchar(200) not null,
	notes				text,
	PRIMARY KEY(import_profile_id)
) ENGINE=InnoDB;

-- END Version 1.3

-- START Version 1.4

ALTER TABLE data_import_profile ADD COLUMN class VARCHAR(255) NOT NULL AFTER profile;

-- END Version 1.4

-- START Version 1.5

ALTER TABLE data_import_field_mapping ADD COLUMN notes VARCHAR(255) NOT NULL;

-- END Version 1.5

-- START Version 1.6

ALTER TABLE data_import_field_mapping CHANGE COLUMN notes notes VARCHAR(255) NULL DEFAULT '';

-- END Version 1.6

-- START Version 1.7

ALTER TABLE data_import_field_mapping CHANGE COLUMN `grouping` grouped TINYINT(3) UNSIGNED NOT NULL DEFAULT 0;

-- END Version 1.7