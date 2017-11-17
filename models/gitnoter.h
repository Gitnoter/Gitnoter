#ifndef GITNOTER_H
#define GITNOTER_H

class Gitnoter
{
public:
    enum GroupType {
        All = 0x000001,
        Recent = 0x000002,
        Unclassified = 0x000003,
        Trash = 0x000004,
        Category = 0x000006,
        Tag = 0x000008
    };

    enum SortBasis {
        Title = 0x001001,
        CreateDate = 0x001002,
        UpdateDate = 0x001003
    };

};

#endif // GITNOTER_H
