#ifndef CUKE_TABLE_HPP_
#define CUKE_TABLE_HPP_

#include <vector>
#include <map>
#include <string>
#include <stdexcept>

namespace cucumber {
namespace internal {

class Table {
private:
    typedef std::vector<std::string> basic_type;
public:
    typedef std::map<std::string,std::string> hash_row_type;
    typedef basic_type columns_type;
    typedef basic_type row_type;
    typedef std::vector<hash_row_type> hashes_type;

    /**
     * @brief addColumn
     * @param column
     *
     * @throws std::runtime_error
     */
    void addColumn(const std::string column);

    /**
     * @brief addRow
     * @param row
     *
     * @throws std::range_error
     * @throws std::runtime_error
     */
    void addRow(const row_type &row);
    const hashes_type & hashes() const;

private:
    hash_row_type buildHashRow(const row_type &row);

    columns_type columns;
    hashes_type rows;
};

}
}

#endif /* CUKE_TABLE_HPP_ */
