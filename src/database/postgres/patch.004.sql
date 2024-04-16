drop function categories_tree_description(category integer);

create or replace function categories_tree_description(category integer) returns text as $$
    declare record record;
    declare ncategory integer;
    declare description_tree text;
    declare delimiter text;
    begin
    ncategory := category;
    description_tree := '';
    delimiter := '';
    loop
        select into record c.category, c.parent_category, c.description
            from categories c
            where c.category = ncategory;
        exit when not found;
        ncategory := record.parent_category;
        description_tree := record.description || delimiter || description_tree;
        delimiter := ' / ';
        end loop;
    return description_tree;
    end;
$$ language 'plpgsql';

