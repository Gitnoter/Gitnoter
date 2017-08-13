PRAGMA foreign_keys = false;

DROP TABLE IF EXISTS "categories";
CREATE TABLE "categories" ("id" INTEGER NOT NULL,"name" TEXT NOT NULL,"create_data" INTEGER NOT NULL,"is_default" INTEGER NOT NULL DEFAULT 0,PRIMARY KEY ("id") );

BEGIN;
INSERT INTO "categories" VALUES (1, '全部日记', 1502625646, 1);
COMMIT;

DROP TABLE IF EXISTS "config";
CREATE TABLE "config" ("version" TEXT);

BEGIN;
INSERT INTO "config" VALUES ('1.0.0');
COMMIT;

DROP TABLE IF EXISTS "notes";
CREATE TABLE "notes" ("id" INTEGER NOT NULL,"title" TEXT,"create_date" INTEGER,"update_date" INTEGER,"body" TEXT,PRIMARY KEY ("id") );

DROP TABLE IF EXISTS "notes_join_categories";
CREATE TABLE "notes_join_categories" ("notes_id" INTEGER NOT NULL,"categories_id" INTEGER NOT NULL,"create_date" INTEGER NOT NULL,CONSTRAINT "fk_notes_join_categories_notes_1" FOREIGN KEY ("notes_id") REFERENCES "notes" ("id"),CONSTRAINT "fk_notes_join_categories_categories_1" FOREIGN KEY ("categories_id") REFERENCES "categories" ("id"));

DROP TABLE IF EXISTS "notes_join_tags";
CREATE TABLE "notes_join_tags" ("notes_id" INTEGER NOT NULL,"tags_id" INTEGER NOT NULL,"create_date" INTEGER NOT NULL,CONSTRAINT "fk_notes_join_tags_notes_1" FOREIGN KEY ("notes_id") REFERENCES "notes" ("id"),CONSTRAINT "fk_notes_join_tags_tags_1" FOREIGN KEY ("tags_id") REFERENCES "tags" ("id"));

DROP TABLE IF EXISTS "tags";
CREATE TABLE "tags" ("id" INTEGER NOT NULL,"name" TEXT NOT NULL,"create_date" INTEGER NOT NULL,PRIMARY KEY ("id") );

PRAGMA foreign_keys = true;
