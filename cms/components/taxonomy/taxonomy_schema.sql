-- Component Schema for taxonomy
--
-- Author: Andy Green
--
-- Each version update must begin with the following:
-- START Version xx
-- and end with
-- END Version xx


-- START Version 1.0

CREATE TABLE `taxonomy`
(
  `taxonomy_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `taxonomy` varchar(200) not null,
  `description` text,
  `associated_classes` varchar(1000) not null default '',
  PRIMARY KEY (`taxonomy_id`)
) ENGINE=InnoDB;


CREATE TABLE `taxonomy_term`
(
  `term_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `term` varchar(200) not null,
  `description` text,
  PRIMARY KEY (`term_id`)
) ENGINE=InnoDB;


CREATE TABLE `taxonomy_term_association`
(
  `term_association_id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `term_id` int(10) not null,
  `class` varchar(100) not null,
  `primary_key` int(10) not null,
  PRIMARY KEY (`term_association_id`)
) ENGINE=InnoDB;

-- END Version 1.0

-- START Version 1.1

ALTER TABLE `taxonomy` CHANGE COLUMN `taxonomy` `taxonomy_name` VARCHAR(200) NOT NULL,
 ADD COLUMN `identifier` VARCHAR(200) NOT NULL AFTER `taxonomy_id`,
 ADD COLUMN `published` TINYINT(3) NOT NULL DEFAULT 0;
 
-- END Version 1.1
 
-- START Version 1.2
 
ALTER TABLE `taxonomy_term` ADD COLUMN `taxonomy_id` INT(10) unsigned NOT NULL DEFAULT 0;
 
-- END Version 1.2

-- START Version 1.3

ALTER TABLE `taxonomy_term_association` ADD COLUMN `taxonomy_id` INT(10) unsigned NOT NULL AFTER `term_association_id`;

-- END Version 1.3

-- START Version 1.4

ALTER TABLE `taxonomy_term_association` CHANGE COLUMN `primary_key` `id` int(10) not null;

-- END Version 1.4

-- START Version 1.5

ALTER TABLE `taxonomy_term` CHANGE COLUMN `description` `definition` text;

-- END Version 1.5

-- START Version 1.6

CREATE OR REPLACE VIEW `taxonomy_term_association_summary` AS
SELECT group_concat(cast(term_id as char(7))) as term_ids, class, id, taxonomy_id 
FROM taxonomy_term_association group by class, id, taxonomy_id;

-- END Version 1.6

-- START Version 1.7

ALTER TABLE `taxonomy` ADD COLUMN `enable_facet_filter` TINYINT(3) UNSIGNED DEFAULT 1;

-- END Version 1.7
