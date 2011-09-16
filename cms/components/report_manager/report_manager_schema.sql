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
