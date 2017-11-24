= GraphDB Design =

== Storage ==
    Elements have a const size, defined in compilation time.
    If sizes of elements can grow beyond that, the content can be a list of element-keys
    that hold the actual data, but that is the responsiblity of the user.
    Pages should be optimized to fit into L1/2 cache, so that scanning through them does
    not cost any additional cache-misses.
    Todo: Decide which cache we want to optimize L1-32Kb, L2-256Kb

    Assuming Max_Page_Size = 32Kb, max element-size = 256Bytes -> Max number of elemtents
    is 127. 

== Pages ==
Pages are the units that actually contain the data.
The max size of page is determined in compilation time and therefore is unneeded to be part
of the page.
Each page starts with some constants followed by an unordered list of Keys contained in this page. 
    0-3: Page magic number
    4-7: PageId (unsigned uint32_t)
    8-11: Current number of elements
    12-31: Reserved for future use.
    32-Max_Page_Size: Data stored
Max number of elements in a page is limited by:
    Max_Page_Size - 32 / Element size.

== Add element to Page ==
    Steps:
    1 - Acquire write-lock for page.    
    2 - Validate page can hold new element,
    3 - Update page count of elements
    4 - Add new element at the end of the list
    5 - Update Bloom filter.
    6 - Sync page to disk.

== Remove element from Page ==
    Steps:
    1 - Acquire write-lock for page.
    2 - Validate page holds element (using Bloom-Filter)
    3 - Scan page to find element.
    4 - If element is found, and is not last, memcpy the last element 
        to the deleted element's position.
    5 - if element was found and deleted, update Bloom-Filter accordingly.
     
        


== Page Splits ==
    A page split is a heavy operation that scans the entire page and splits the elements into 2 new pages of equal size. System assumes that due to uniform distribution of the Key-hashing function, the pages should end up with somewhat similar number of elements.

== Page Joins == 
    Once elements have     
