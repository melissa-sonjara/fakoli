--
-- START Version 1.1
--

CREATE TABLE `custom_report` (
  `report_id` INTEGER UNSIGNED NOT NULL AUTO_INCREMENT,
  `manager_class` VARCHAR(100) NOT NULL,
  `title` VARCHAR(200) NOT NULL,
  `description` TEXT,
  `configuration` TEXT,
  PRIMARY KEY (`report_id`)
)
ENGINE = InnoDB;

--
-- END Version 1.1
--

-- START Version 1.2

ALTER TABLE `custom_report` CHANGE COLUMN `configuration` `configuration` longtext;
ALTER TABLE `custom_report` ADD COLUMN `user_id` int(10) NOT NULL DEFAULT '0';

-- END Version 1.2

-- START Version 1.3

ALTER TABLE `custom_report` ADD COLUMN `shared` tinyint(3) not null default 0;

-- END Version 1.3