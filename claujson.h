﻿

#pragma once

#include "fmt/format.h"
#include "fmt/compile.h"

#include <iostream>
#include "simdjson.h" // modified simdjson 

#include <map>
#include <vector>
#include <string>
#include <set>
#include <fstream>
#include <iomanip>

//#include <Windows.h>

#define INLINE inline


namespace claujson {


	using STRING = std::string;

	class UserType;


	//UserType* ChkPool(UserType*& node, PoolManager& manager);

	class Data {
	private:
		union {
			long long _int_val = 0;
			unsigned long long _uint_val;
			double _float_val;
			std::string* _str_val; // const
		};

		simdjson::internal::tape_type _type = simdjson::internal::tape_type::NONE;
	
	public:

		simdjson::internal::tape_type type() const {
			return _type;
		}

		long long int_val() const {
			return _int_val;
		}

		unsigned long long uint_val() const {
			return _uint_val;
		}

		double float_val() const {
			return _float_val;
		}

		long long& int_val()  {
			return _int_val;
		}

		unsigned long long& uint_val()  {
			return _uint_val;
		}

		double& float_val()  {
			return _float_val;
		}

		//bool is_key = false;
	private:
	public:
		void clear() {
			//is_key = false;
			
			if (_type == simdjson::internal::tape_type::STRING) {
				delete _str_val;
				_str_val = nullptr;
			}
			else {
				_int_val = 0;
			}

			_type = simdjson::internal::tape_type::NONE;
		}

		const std::string* get_str_val() const {
			// type check...
			return _str_val;
		}

		void set_int(long long x) {
			if (_type == simdjson::internal::tape_type::STRING) {
				delete _str_val;
			}
			_int_val = x;
			_type = simdjson::internal::tape_type::INT64;
		}

		void set_uint(unsigned long long x) {
			if (_type == simdjson::internal::tape_type::STRING) {
				delete _str_val;
			}
			_uint_val = x;
			_type = simdjson::internal::tape_type::UINT64;
		}

		void set_float(double x) {
			if (_type == simdjson::internal::tape_type::STRING) {
				delete _str_val;
			}
			_float_val = x;

			_type = simdjson::internal::tape_type::DOUBLE;
		}

		void set_str(const char* str, size_t len) {
			if (_type != simdjson::internal::tape_type::STRING) {
				_str_val = new std::string(str, len);
			}
			else {
				_str_val->assign(str, len);
			}
			_type = simdjson::internal::tape_type::STRING;
		}

		void set_type(simdjson::internal::tape_type type) {
			this->_type = type;
		}

	public:
		virtual ~Data() {
			if (_type == simdjson::internal::tape_type::STRING && _str_val) {
				//std::cout << "chk";
				delete _str_val;
				_str_val = nullptr;
			}
		}

		Data(const Data& other)
			: _type(other._type) //, is_key(other.is_key) 
		{
			if (_type == simdjson::internal::tape_type::STRING) {
				_str_val = new std::string(*other._str_val);
				
			}
			else {
				_int_val = other._int_val;
			}
		}

		Data(Data&& other) noexcept
			: _type(other._type) //, is_key(other.is_key) 
		{
			
			if (_type == simdjson::internal::tape_type::STRING) {
				_str_val = other._str_val;
				other._str_val = nullptr;
				other._type = simdjson::internal::tape_type::NONE;
			}
			else {
				std::swap(_int_val, other._int_val);
			}
		}

		Data() : _int_val(0), _type(simdjson::internal::tape_type::NONE) { } 

		bool operator==(const Data& other) const {
			if (this->_type == other._type) {
				switch (this->_type) {
				case simdjson::internal::tape_type::STRING:
					return *this->_str_val == *other._str_val;
					break;
				}
				return true;
			}
			return false;
		}

		bool operator<(const Data& other) const {
			if (this->_type == other._type) {
				switch (this->_type) {
				case simdjson::internal::tape_type::STRING:
					return *this->_str_val < *other._str_val;
					break;
				}
			}
			return false;
		}

		Data& operator=(const Data& other) {
			if (this == &other) {
				return *this;
			}

			if (this->_type != simdjson::internal::tape_type::STRING && other._type == simdjson::internal::tape_type::STRING) {
				this->_str_val = new std::string();
			}
			else if (this->_type == simdjson::internal::tape_type::STRING && other._type != simdjson::internal::tape_type::STRING) {
				delete this->_str_val;
			}
			
			this->_type = other._type;

			if (this->_type == simdjson::internal::tape_type::STRING) {
				set_str(other._str_val->c_str(), other._str_val->size());
			}
			else {
				this->_int_val = other._int_val;
			}

			return *this;
		}


		Data& operator=(Data&& other) noexcept {
			if (this == &other) {
				return *this;
			}
			
			std::swap(this->_type, other._type);
			std::swap(this->_int_val, other._int_val);

			return *this;
		}

		friend std::ostream& operator<<(std::ostream& stream, const Data& data) {

			switch (data._type) {
			case simdjson::internal::tape_type::INT64:
				stream << data._int_val;
				break;
			case simdjson::internal::tape_type::UINT64:
				stream << data._uint_val;
				break;
			case simdjson::internal::tape_type::DOUBLE:
				stream << data._float_val;
				break;
			case simdjson::internal::tape_type::STRING:
				stream << (*data._str_val);
				break;
			case simdjson::internal::tape_type::TRUE_VALUE:
				stream << "true";
				break;
			case simdjson::internal::tape_type::FALSE_VALUE:
				stream << "false";
				break;
			case simdjson::internal::tape_type::NULL_VALUE:
				stream << "null";
				break;
			case simdjson::internal::tape_type::START_ARRAY:
				stream << "[";
				break;
			case simdjson::internal::tape_type::START_OBJECT:
				stream << "{";
				break;
			case simdjson::internal::tape_type::END_ARRAY:
				stream << "]";
				break;
			case simdjson::internal::tape_type::END_OBJECT:
				stream << "}";
				break;
			}

			return stream;
		}
	};
}

#if SIMDJSON_IMPLEMENTATION_ICELAKE
#define SIMDJSON_IMPLEMENTATION icelake
#elif SIMDJSON_IMPLEMENTATION_HASWELL
#define SIMDJSON_IMPLEMENTATION haswell //
#elif SIMDJSON_IMPLEMENTATION_WESTMERE
#define SIMDJSON_IMPLEMENTATION westmere
#elif SIMDJSON_IMPLEMENTATION_ARM64
#define SIMDJSON_IMPLEMENTATION arm64
#elif SIMDJSON_IMPLEMENTATION_PPC64
#define SIMDJSON_IMPLEMENTATION ppc64
#elif SIMDJSON_IMPLEMENTATION_FALLBACK
#define SIMDJSON_IMPLEMENTATION fallback
#else
#error "All possible implementations (including fallback) have been disabled! simdjson will not run."
#endif

namespace simdjson {
	namespace SIMDJSON_IMPLEMENTATION {


		struct Writer {

			/** The next place to write to tape */
			uint64_t* next_tape_loc;

			/** Write a signed 64-bit value to tape. */
			simdjson_really_inline void append_s64(int64_t value) noexcept;

			/** Write an unsigned 64-bit value to tape. */
			simdjson_really_inline void append_u64(uint64_t value) noexcept;

			/** Write a double value to tape. */
			simdjson_really_inline void append_double(double value) noexcept;

			/**
			 * Append a tape entry (an 8-bit type,and 56 bits worth of value).
			 */
			simdjson_really_inline void append(uint64_t val, internal::tape_type t) noexcept;

			/**
			 * Skip the current tape entry without writing.
			 *
			 * Used to skip the start of the container, since we'll come back later to fill it in when the
			 * container ends.
			 */
			simdjson_really_inline void skip() noexcept;

			/**
			 * Skip the number of tape entries necessary to write a large u64 or i64.
			 */
			simdjson_really_inline void skip_large_integer() noexcept;

			/**
			 * Skip the number of tape entries necessary to write a double.
			 */
			simdjson_really_inline void skip_double() noexcept;

			/**
			 * Write a value to a known location on tape.
			 *
			 * Used to go back and write out the start of a container after the container ends.
			 */
			simdjson_really_inline static void write(uint64_t& tape_loc, uint64_t val, internal::tape_type t) noexcept;

		private:
			/**
			 * Append both the tape entry, and a supplementary value following it. Used for types that need
			 * all 64 bits, such as double and uint64_t.
			 */
			template<typename T>
			simdjson_really_inline void append2(uint64_t val, T val2, internal::tape_type t) noexcept;
		}; // struct number_writer

		simdjson_really_inline void Writer::append_s64(int64_t value) noexcept {
			append2(0, value, internal::tape_type::INT64);
		}

		simdjson_really_inline void Writer::append_u64(uint64_t value) noexcept {
			append(0, internal::tape_type::UINT64);
			*next_tape_loc = value;
			next_tape_loc++;
		}

		/** Write a double value to tape. */
		simdjson_really_inline void Writer::append_double(double value) noexcept {
			append2(0, value, internal::tape_type::DOUBLE);
		}

		simdjson_really_inline void Writer::skip() noexcept {
			next_tape_loc++;
		}

		simdjson_really_inline void Writer::skip_large_integer() noexcept {
			next_tape_loc += 2;
		}

		simdjson_really_inline void Writer::skip_double() noexcept {
			next_tape_loc += 2;
		}

		simdjson_really_inline void Writer::append(uint64_t val, internal::tape_type t) noexcept {
			*next_tape_loc = val | ((uint64_t(char(t))) << 56);
			next_tape_loc++;
		}

		template<typename T>
		simdjson_really_inline void Writer::append2(uint64_t val, T val2, internal::tape_type t) noexcept {
			append(val, t);
			static_assert(sizeof(val2) == sizeof(*next_tape_loc), "Type is not 64 bits!");
			memcpy(next_tape_loc, &val2, sizeof(val2));
			next_tape_loc++;
		}

		simdjson_really_inline void Writer::write(uint64_t& tape_loc, uint64_t val, internal::tape_type t) noexcept {
			tape_loc = val | ((uint64_t(char(t))) << 56);
		}

	};

	// todo 
	//- add bool is_key ...
	INLINE claujson::Data& Convert(::claujson::Data& data, uint64_t idx, uint64_t idx2, uint64_t len, bool key,
		char* buf, uint8_t* string_buf, uint64_t id) {
		data.clear();

		uint32_t string_length;

		switch (buf[idx]) {
		case '"':
		{ // need buf_length?

			//if (key) {
				//data.is_key = true;
			//}

			if (auto* x = simdjson::SIMDJSON_IMPLEMENTATION::stringparsing::parse_string((uint8_t*)&buf[idx] + 1,
				&string_buf[idx]); x == nullptr) {
				std::cout << "ERROR in string\n";
				// error processing?
				exit(1);
			}
			else {
				*x = '\0';
				string_length = uint32_t(x - &string_buf[idx]);
			}

			// chk token_arr_start + i + 1 >= imple->n_structural_indexes...
			data.set_str(reinterpret_cast<char*>(&string_buf[idx]), string_length);
		}
		break;
		case 't':
		{
			if (!simdjson::SIMDJSON_IMPLEMENTATION::atomparsing::is_valid_true_atom(reinterpret_cast<uint8_t*>(&buf[idx]), idx2 - idx)) {
				std::cout << idx2 << "\n";
				exit(1);
			}

			data.set_type((simdjson::internal::tape_type)buf[idx]);
		}
		break;
		case 'f':
			if (!simdjson::SIMDJSON_IMPLEMENTATION::atomparsing::is_valid_false_atom(reinterpret_cast<uint8_t*>(&buf[idx]), idx2 - idx)) {
				exit(1);
			}

			data.set_type((simdjson::internal::tape_type)buf[idx]);
			break;
		case 'n':
			if (!simdjson::SIMDJSON_IMPLEMENTATION::atomparsing::is_valid_null_atom(reinterpret_cast<uint8_t*>(&buf[idx]), idx2 - idx)) {
				exit(1);
			}

			data.set_type((simdjson::internal::tape_type)buf[idx]);
			break;
		case '-':
		case '0':
		case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
		{
			std::unique_ptr<uint8_t[]> copy;

			uint64_t temp[2];
			SIMDJSON_IMPLEMENTATION::Writer writer{ temp };
			uint8_t* value = reinterpret_cast<uint8_t*>(buf + idx);

			if (id == 0) {
				copy = std::unique_ptr<uint8_t[]>(new (std::nothrow) uint8_t[idx2 - idx + SIMDJSON_PADDING]);
				if (copy.get() == nullptr) { exit(1); }
				std::memcpy(copy.get(), &buf[idx], idx2 - idx);
				std::memset(copy.get() + idx2 - idx, ' ', SIMDJSON_PADDING);
				value = copy.get();
			}

			if (auto x = SIMDJSON_IMPLEMENTATION::numberparsing::parse_number<SIMDJSON_IMPLEMENTATION::Writer>(value, writer)
				; x != simdjson::error_code::SUCCESS) {
				std::cout << "parse number error. " << x << "\n";
				exit(1);
			}

			long long int_val = 0;
			unsigned long long uint_val = 0;
			double float_val = 0;

			switch (static_cast<simdjson::internal::tape_type>(temp[0] >> 56)) {
			case simdjson::internal::tape_type::INT64:
				memcpy(&int_val, &temp[1], sizeof(uint64_t));

				data.set_int(int_val);
				break;
			case simdjson::internal::tape_type::UINT64:
				memcpy(&uint_val, &temp[1], sizeof(uint64_t));
				
				data.set_uint(uint_val);
				break;
			case simdjson::internal::tape_type::DOUBLE:
				memcpy(&float_val, &temp[1], sizeof(uint64_t));
				
				data.set_float(float_val);
				break;
			}
			/*
			if (buf[idx] == '0' && isdigit(buf[idx + 1])) {
				exit(1);
				//
			}
			//if (std::from_chars(value, value2 + 1, data.int_val).ec != std::errc()) {
			//	if (std::from_chars(value, value2 + 1, data.uint_val).ec != std::errc()) {
			if (fast_float::from_chars(&buf[idx], buf.get() + idx2, data.float_val).ec != std::errc()) {
				std::cout << "Error.. not number. \n";
			}
			else {
				data.type = simdjson::internal::tape_type::DOUBLE;
			}
			//	}
			//	else {
			//		data.type = simdjson::internal::tape_type::UINT64;
			///	}
			//}
			//else {
			//	data.type = simdjson::internal::tape_type::INT64;
			//}
			*/
			break;
		}
		default:
			std::cout << "convert error : " << (int)buf[idx] << " " << buf[idx] << "\n";
			exit(1);
		}
		return data;
	}
}


namespace claujson {

	class ItemType {
	public:
		Data key;
		Data data;
		bool has_key = false;
	public:
		ItemType() { }

		ItemType(const Data& key, const Data& data, bool has_key) : key(key), data(data), has_key(has_key)
		{
		
		}

		ItemType(Data&& key, Data&& data, bool has_key) noexcept : key(std::move(key)), data(std::move(data)), has_key(has_key)
		{
			//
		}

		ItemType(const ItemType& other) : key(other.key), data(other.data),  has_key(other.has_key) {
			//
		}
		ItemType(ItemType&& other) noexcept : key(std::move(other.key)), data(std::move(other.data)), has_key(other.has_key)
		{
			//
		}

		ItemType& operator=(ItemType&& other) noexcept {
			std::swap(key, other.key);
			std::swap(data, other.data);
			std::swap(has_key, other.has_key);

			return *this;
		}

		ItemType& operator=(const ItemType& other) {
			key = (other.key);
			data = (other.data);
			has_key = other.has_key;

			return *this;
		}
	};

	class UserType {
		

	public:

		INLINE static UserType* make_object(ItemType&& x) {
			UserType* temp = new UserType();
			new (temp) UserType(std::move(x), 0);
			return temp;
		}

		INLINE static UserType* make_array(ItemType&& x) {
			UserType* temp = new UserType();
			new (temp) UserType(std::move(x), 1);
			return temp;
		}


		UserType* clone() const {
			//std::cout << "clone";

			UserType* temp = new UserType(this->value, this->type);

			temp->type = this->type;

			temp->parent = nullptr; // chk!

			temp->data.reserve(this->data.size());

			for (auto& x : this->data) {
				temp->data.push_back(x->clone());
				temp->data.back()->parent = temp;
			}

			return temp;
		}

	private:

		ItemType value; // equal to key

		std::vector<UserType*> data;

		UserType* parent = nullptr; // 
		
		int type = -1; // 0 - object, 1 - array, 2 - virtual object, 3 - virtual array, 4 - item, -1 - root, -2 - only in parse... -4 ..
		
	public:
		//INLINE const static size_t npos = -1; // ?
		// chk type?
		bool operator<(const UserType& other) const {
			return *(value.key.get_str_val()) < *(other.value.key.get_str_val());
		}
		bool operator==(const UserType& other) const {
			return *(value.key.get_str_val()) == *(other.value.key.get_str_val());
		}

	public:

		INLINE const std::vector<UserType*>& get_data() const { return data; }
		INLINE std::vector<UserType*>& get_data() { return data; }

		// find_ut..
		UserType* find_ut(std::string_view key) {
			for (size_t i = 0; i < data.size(); ++i) {
				if (data[i]->is_user_type() && *data[i]->value.key.get_str_val() == key) {
					return data[i];
				}
			}
			return nullptr;
		}

		const UserType* find_ut(std::string_view key) const {
			for (size_t i = 0; i < data.size(); ++i) {
				if (data[i]->is_user_type() && *data[i]->value.key.get_str_val() == key) {
					return data[i];
				}
			}
			return nullptr;
		}

	public:
		UserType(const UserType& other)
			: value(other.value),
			type(other.type) //, parent(other.parent) //
		{
			this->data.reserve(other.data.size());
			for (auto& x : other.data) {
				this->data.push_back(x->clone());
				this->data.back()->parent = this;
			}
		}

		//todo - fix? for type, parent, std::swap?
		UserType(UserType&& other) noexcept {
			std::swap(this->value, other.value);
			std::swap(this->data, other.data);
			
			// chk
			for (auto& x : data) {
				x->parent = this;
			}

			std::swap(type, other.type);
			//parent = std::move(other.parent);//
		}

		UserType& operator=(const UserType& other) {
			if (this == &other) {
				return *this;
			}

			value = (other.value);
			for (auto& x : data) {
				if (x) {
					delete x;
					x = nullptr;
				}
			}
			data.clear();
			data.reserve(other.get_data_size());
			for (auto& x : other.data) {
				if (x) {
					data.push_back(x->clone());
					data.back()->parent = this;
				}
			}
			//data = (other.data);
			type = (other.type);
			//parent = (other.parent);//

			return *this;
		}

		UserType& operator=(UserType&& other) noexcept {
			if (this == &other) {
				return *this;
			}

			value = std::move(other.value);
			std::swap(data, other.data); //
			for (auto& x : data) {
				x->parent = this;
			}
			std::swap(type, other.type);
			//parent = std::move(other.parent);//

			return *this;
		}

		const ItemType& get_value() const { return value; }


	private:
		void LinkUserType(UserType* ut) // friend?
		{
			if (this->is_object() && !ut->value.has_key) {
				exit(1);
			}
			else if (this->is_array() && ut->value.has_key) {
				exit(1);
			}

			data.push_back(ut);

			ut->parent = this;
		}
		void LinkItemType(UserType* item) {
			if (this->is_object() && !item->value.has_key) {
				exit(1);
			}
			else if (this->is_array() && item->value.has_key) {
				exit(1);
			}

			this->data.push_back(item);
		}

	private:
		UserType(Data&& key, Data&& value, bool has_key, int type) noexcept : value(std::move(key), std::move(value), has_key), type(type) {
			//
		}
		UserType(const Data& key, const Data& value, bool has_key, int type) noexcept : value((key), (value), has_key), type(type) {
			//
		}

		UserType(ItemType&& value, int type) noexcept : value(std::move(value)), type(type)
		{
			//
		}

		UserType(const ItemType& value, int type) noexcept : value(value), type(type)
		{
			//
		}
	public:
		UserType() noexcept : type(-1) {
			//
		}
		virtual ~UserType() noexcept {
			for (size_t i = 0; i < data.size(); ++i) {
				if (data[i]) {
					delete data[i];
					data[i] = nullptr;
				}
			}
		}
	public:

		bool is_object() const {
			return type == 0 || type == 2;
		}

		bool is_array() const {
			return type == 1 || type == 3 || type == -1;
		}

		bool is_in_root() const {
			return get_parent()->type == -1;
		}

		bool is_item_type() const {
			return type == 4;
		}

		bool is_user_type() const {
			return is_object() || is_array();
		}

		bool is_root() const {
			return type == -1;
		}

		// name key check?
		void add_object_element(const claujson::Data& name, const claujson::Data& data) {
			// todo - chk this->type == 0 (object) but name is empty
			// todo - chk this->type == 1 (array) but name is not empty.

			if (this->type == 1) {
				throw "Error add object element to array in add_object_element ";
			}
			if (this->type == -1 && this->data.size() >= 1) {
				throw "Error not valid json in add_object_element";
			}

			if (name.type() != simdjson::internal::tape_type::STRING) {
				exit(1);
			}

			this->data.push_back(make_object(ItemType(name, data, true)));
		}

		void add_array_element(const claujson::Data& data) {
			// todo - chk this->type == 0 (object) but name is empty
			// todo - chk this->type == 1 (array) but name is not empty.

			if (this->type == 0) {
				throw "Error add object element to array in add_array_element ";
			}
			if (this->type == -1 && this->data.size() >= 1) {
				throw "Error not valid json in add_array_element";
			}

			this->data.push_back(make_array(ItemType(Data(), data, false))); // (Type*)make_item_type(std::move(temp), data));
		}

		void remove_all(UserType* ut) {
			for (size_t i = 0; i < ut->data.size(); ++i) {
				if (ut->data[i]) {
					delete ut->data[i];
					ut->data[i] = nullptr;
				}
			}
			ut->data.clear();
		}

		void remove_all() {
			remove_all(this);
		}

	public:

		void add_object_with_key(UserType* object) {
			const auto& name = object->value;

			if (is_array()) {
				throw "Error in add_object_with_key";
			}

			if (this->type == -1 && this->data.size() >= 1) {
				throw "Error not valid json in add_object_with_key";
			}


			if (object->value.key.type() != simdjson::internal::tape_type::STRING) {
				exit(1);
			}

			this->data.push_back(object);
			((UserType*)this->data.back())->parent = this;
		}

		void add_array_with_key(UserType* _array) {
			const auto& name = _array->value;

			if (is_array()) {
				throw "Error in add_array_with_key";
			}

			if (this->type == -1 && this->data.size() >= 1) {
				throw "Error not valid json in add_array_with_key";
			}

			if (_array->value.key.type() != simdjson::internal::tape_type::STRING) {
				exit(1);
			}

			this->data.push_back(_array);
			((UserType*)this->data.back())->parent = this;
		}

		void add_object_with_no_key(UserType* object) {
			const Data& name = object->value.key;

			if (is_object()) {
				throw "Error in add_object_with_no_key";
			}

			if (this->type == -1 && this->data.size() >= 1) {
				throw "Error not valid json in add_object_with_no_key";
			}

			if (object->value.key.type() == simdjson::internal::tape_type::STRING) {
				exit(1);
			}

			this->data.push_back(object);
			((UserType*)this->data.back())->parent = this;
		}

		void add_array_with_no_key(UserType* _array) {
			const Data& name = _array->value.key;

			if (is_object()) {
				throw "Error in add_array_with_no_key";
			}

			if (this->type == -1 && this->data.size() >= 1) {
				throw "Error not valid json in add_array_with_no_key";
			}

			if (_array->value.key.type() == simdjson::internal::tape_type::STRING) {
				exit(1);
			}

			this->data.push_back(_array);
			((UserType*)this->data.back())->parent = this;
		}

		void reserve_data_list(size_t len) {
			data.reserve(len);
		}

	private:

		INLINE void add_user_type(UserType* ut) {
			this->data.push_back(ut);
			ut->parent = this;
		}


		INLINE static UserType make_none() {
			ItemType temp;

			UserType ut(std::move(temp), -2);

			return ut;
		}

		INLINE bool is_virtual() const {
			return type == 2 || type == 3;
		}

		INLINE static UserType make_virtual_object() {
			UserType ut;
			ut.type = 2;
			return ut;
		}

		INLINE static UserType make_virtual_array() {
			UserType ut;
			ut.type = 3;
			return ut;
		}

		INLINE void add_user_type(int64_t idx, int64_t idx2, int64_t len, char* buf,
			uint8_t* string_buf, int type, uint64_t id) {
			// todo - chk this->type == 0 (object) but name is empty
			// todo - chk this->type == 1 (array) but name is not empty.
			// todo - chk this->type == -1 .. one object or one array or data(true or false or null or string or number).

			if (is_array()) {
				std::cout << "object \n";
				exit(1);
			}


			//if (this->type == -1 && this->data.size() >= 1) {
			//	throw "Error not valid json in add_user_type";
			//}
			
			Data temp;
			
			simdjson::Convert(temp, idx, idx2, len, true, buf, string_buf, id);


			if (temp.type() != simdjson::internal::tape_type::STRING) {
				exit(1);
			}

			this->data.push_back(new UserType(ItemType(std::move(temp), Data(), true), type));

			((UserType*)this->data.back())->parent = this;
		}

		INLINE void add_user_type(int type) {
			// todo - chk this->type == 0 (object) but name is empty
			// todo - chk this->type == 1 (array) but name is not empty.
			// todo - chk this->type == -1 .. one object or one array or data(true or false or null or string or number).

			if (is_object()) {
				std::cout << "array \n";
				exit(1);
			}

			//if (this->type == -1 && this->data.size() >= 1) {
			//	throw "Error not valid json in add_user_type";
			//}

			this->data.push_back(new UserType(ItemType(), type));

			((UserType*)this->data.back())->parent = this;

		}

		// add item_type in object? key = value
		INLINE void add_item_type(int64_t idx11, int64_t idx12, int64_t len1, int64_t idx21, int64_t idx22, int64_t len2,
			char* buf, uint8_t* string_buf, uint64_t id, uint64_t id2) {
			// todo - chk this->type == 0 (object) but name is empty
			// todo - chk this->type == 1 (array) but name is not empty.

			if (is_array()) {
				std::cout << "object \n";
				exit(1);
			}

			//if (this->type == -1 && this->data.size() >= 1) {
			//	throw "Error not valid json in add_item_type";
			//}

			{
				Data temp;
				Data temp2;

				simdjson::Convert(temp, idx11, idx12, len1, true, buf, string_buf, id);
				simdjson::Convert(temp2, idx21, idx22, len2, false, buf, string_buf, id2);

				if (temp.type() != simdjson::internal::tape_type::STRING) {
					exit(1);
				}


				this->data.push_back(new UserType(ItemType(std::move(temp), std::move(temp2), true), 4));
			}
		}

		INLINE void add_item_type(int64_t idx21, int64_t idx22, int64_t len2,
			char* buf, uint8_t* string_buf, uint64_t id) {
			// todo - chk this->type == 0 (object) but name is empty
			// todo - chk this->type == 1 (array) but name is not empty.

			if (is_object()) {
				std::cout << "array \n";
				exit(1);
			}


			//if (this->type == -1 && this->data.size() >= 1) {
			//	throw "Error not valid json in add_item_type";
			//}

			Data temp2;

			simdjson::Convert(temp2, idx21, idx22, len2, false, buf, string_buf, id);

			this->data.push_back(new UserType(ItemType(Data(), std::move(temp2), false), 4));
		}

		INLINE void add_item_type(const Data& name, const claujson::Data& data) {
			// todo - chk this->type == 0 (object) but name is empty
			// todo - chk this->type == 1 (array) but name is not empty.

			if (is_array()) {
				std::cout << "object \n";
				exit(1);
			}

			//	if (this->type == -1 && this->data.size() >= 1) {
				//	throw "Error not valid json in add_item_type";
			//	}
			if (name.type() != simdjson::internal::tape_type::STRING) {
				exit(1);
			}


			this->data.push_back(new UserType(name, data, true, 4));
		}

		INLINE void add_item_type(const claujson::Data& data) {
			// todo - chk this->type == 0 (object) but name is empty
			// todo - chk this->type == 1 (array) but name is not empty.

			//if (this->type == -1 && this->data.size() >= 1) {
			//	throw "Error not valid json in add_item_type";
			//}
			if (is_object()) {
				std::cout << "array \n";
				exit(1);
			}

			this->data.push_back(new UserType(Data(), data, false, 4));
		}

	public:

		UserType*& get_data_list(size_t idx) {
			return this->data[idx];
		}
		const UserType* const& get_data_list(size_t idx) const {
			return this->data[idx];
		}

		size_t get_data_size() const {
			return this->data.size();
		}


		void remove_data_list(size_t idx) {
			delete data[idx];
			data[idx] = nullptr;
			data.erase(data.begin() + idx);
		}


		UserType* get_parent() {
			return parent;
		}

		const UserType* get_parent() const {
			return parent;
		}

		friend class LoadData;
	};


	
}


namespace claujson {

	inline simdjson::internal::tape_type get_type(unsigned char x) { // todo...
		switch (x) {
		case '-':
		case '0':
		case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
			return simdjson::internal::tape_type::DOUBLE; // number?
			break;
		case '"':
		case 't':
		case 'f':
		case 'n':
		case '{':
		case '[':
		case '}':
		case ']':
			return	(simdjson::internal::tape_type)(x);
			break;
		case ':':
		case ',':

			return	(simdjson::internal::tape_type)(x);
			break;
		}
		return simdjson::internal::tape_type::NONE;
	}

	class LoadData
	{
	public:

		static int Merge(class UserType* next, class UserType* ut, class UserType** ut_next)
		{

			// check!!
			while (ut->get_data_size() >= 1
				&& (ut->get_data_list(0)->is_user_type()) && (ut->get_data_list(0))->is_virtual())
			{
				ut = (UserType*)ut->get_data_list(0);
			}

			bool chk_ut_next = false;

			while (true) {

				class UserType* _ut = ut;
				class UserType* _next = next;

				if (_next->is_array() && _ut->is_object()) {
					exit(1);
				}
				if (_next->is_object() && _ut->is_array()) {
					exit(1);
				}


				if (ut_next && _ut == *ut_next) {
					*ut_next = _next;
					chk_ut_next = true;

					std::cout << "chked in merge...\n";
				}



				size_t _size = _ut->get_data_size(); // bug fix.. _next == _ut?
				for (size_t i = 0; i < _size; ++i) {
					if (_ut->get_data_list(i)->is_user_type()) {
						if (((UserType*)_ut->get_data_list(i))->is_virtual()) {
							//_ut->get_user_type_list(i)->used();
						}
						else {
							_next->LinkUserType(_ut->get_data_list(i));
							_ut->get_data_list(i) = nullptr;
						}
					}
					else { // item type.
						_next->LinkItemType((_ut->get_data_list(i)));
						_ut->get_data_list(i) = nullptr;
					}
				}

				_ut->remove_all();

				ut = ut->get_parent();
				next = next->get_parent();


				if (next && ut) {
					//
				}
				else {
					// right_depth > left_depth
					if (!next && ut) {
						return -1;
					}
					else if (next && !ut) {
						return 1;
					}

					return 0;
				}
			}
		}

	private:

		struct Test { // need to rename.
			int64_t idx;
			int64_t idx2;
			int64_t len;
			uint64_t id;
			bool is_key = false;
		};

		static bool __LoadData(char* buf, size_t buf_len,
			uint8_t* string_buf,
			simdjson::internal::dom_parser_implementation* imple,
			int64_t token_arr_start, size_t token_arr_len, class UserType* _global,
			int start_state, int last_state, class UserType** next, int* err, int no)
		{
			int a = clock();

			std::vector<Test> Vec;

			if (token_arr_len <= 0) {
				*next = nullptr;
				return false;
			}

			class UserType& global = *_global;

			int state = start_state;
			size_t braceNum = 0;
			std::vector< class UserType* > nestedUT;

			nestedUT.reserve(10);
			nestedUT.push_back(&global);

			int64_t count = 0;

			Test key; bool is_before_comma = false;
			bool is_now_comma = false;

			if (token_arr_start > 0) {
				const simdjson::internal::tape_type before_type = // next_type
					get_type(buf[imple->structural_indexes[token_arr_start - 1]]);

				is_before_comma = before_type == simdjson::internal::tape_type::COMMA;
			}


			for (int64_t i = 0; i < token_arr_len; ++i) {

				const simdjson::internal::tape_type type = get_type(buf[imple->structural_indexes[token_arr_start + i]]);

				switch (state)
				{
				case 0:
				{
					if (is_before_comma && type == simdjson::internal::tape_type::COMMA) {
						std::cout << "before is comma\n";
						exit(1);
						//
					}


					if (token_arr_start + i > 1) {
						const simdjson::internal::tape_type before_type =
							get_type(buf[imple->structural_indexes[token_arr_start + i - 1]]);

						if (before_type == simdjson::internal::tape_type::START_ARRAY || before_type == simdjson::internal::tape_type::START_OBJECT) {
							is_now_comma = false; //std::cout << "2-i " << i << "\n";
						}
					}

					if (is_before_comma) {
						is_now_comma = false;
					}

					if (!is_now_comma && type == simdjson::internal::tape_type::COMMA) {
						std::cout << "now is not comma\n";
						exit(1);
						//
					}
					if (is_now_comma && type != simdjson::internal::tape_type::COMMA) {
						std::cout << "is now comma... but not..\n";
						exit(1);
					}


					is_before_comma = type == simdjson::internal::tape_type::COMMA;

					if (type == simdjson::internal::tape_type::COMMA) {
						if (token_arr_start + i + 1 < imple->n_structural_indexes) {
							const simdjson::internal::tape_type _type =
								get_type(buf[imple->structural_indexes[token_arr_start + i + 1]]);

							if (_type == simdjson::internal::tape_type::END_ARRAY || _type == simdjson::internal::tape_type::END_OBJECT) {
								exit(1);
								//
							}
							else if (_type == simdjson::internal::tape_type::COLON) {
								exit(1);
							}

							continue;
						}
						else {
							exit(1);
						}
					}

					if (type == simdjson::internal::tape_type::COLON) {
						exit(1);
						//
					}

					switch (type) {
					case simdjson::internal::tape_type::END_ARRAY:
					case simdjson::internal::tape_type::END_OBJECT:

					case simdjson::internal::tape_type::STRING:
					case simdjson::internal::tape_type::INT64:
					case simdjson::internal::tape_type::UINT64:
					case simdjson::internal::tape_type::DOUBLE:
					case simdjson::internal::tape_type::TRUE_VALUE:
					case simdjson::internal::tape_type::FALSE_VALUE:
					case simdjson::internal::tape_type::NULL_VALUE:
					case simdjson::internal::tape_type::NONE:
						is_now_comma = true; // std::cout << "3-i " << i << "\n";
						break;
					}

					if (token_arr_start + i + 1 < imple->n_structural_indexes) {
						const simdjson::internal::tape_type _type =
							get_type(buf[imple->structural_indexes[token_arr_start + i + 1]]);

						//std::cout << "chk " << (int)get_type(buf[imple->structural_indexes[token_arr_start + i + 1]]) << "\n";

						if (_type == simdjson::internal::tape_type::END_ARRAY || _type == simdjson::internal::tape_type::END_OBJECT) {
							is_now_comma = false; //std::cout << "1-i " << i << "\n";
							//
						}
					}
					else {
						is_now_comma = false;
					}

					// Left 1
					//else
					if (type == simdjson::internal::tape_type::START_OBJECT ||
						type == simdjson::internal::tape_type::START_ARRAY) { // object start, array start



						if (!Vec.empty()) {

							if (Vec[0].is_key) {
								nestedUT[braceNum]->reserve_data_list(nestedUT[braceNum]->get_data_size() + Vec.size() / 2);

								if (Vec.size() % 2 == 1) {
									std::cout << "Vec.size()%2==1\n";
									exit(1);
								}

								for (size_t x = 0; x < Vec.size(); x += 2) {
									if (!Vec[x].is_key) {
										std::cout << "vec[x].is not key\n";
										exit(1);
									}
									if (Vec[x + 1].is_key) {
										std::cout << "vec[x].is key\n";
										exit(1);
									}
									nestedUT[braceNum]->add_item_type( (Vec[x].idx), Vec[x].idx2, Vec[x].len,
										(Vec[x + 1].idx), Vec[x + 1].idx2, Vec[x + 1].len,
										buf, string_buf, Vec[x].id, Vec[x + 1].id);
									//++pool;
								}
							}
							else {
								nestedUT[braceNum]->reserve_data_list(nestedUT[braceNum]->get_data_size() + Vec.size());

								for (size_t x = 0; x < Vec.size(); x += 1) {
									if (Vec[x].is_key) {
										std::cout << "Vec[x].iskey\n";
										exit(1);
									}
									nestedUT[braceNum]->add_item_type( (Vec[x].idx), Vec[x].idx2, Vec[x].len, buf, string_buf, Vec[x].id);
									//++pool;
								}
							}

							Vec.clear();
						}

						if (key.is_key) {
							nestedUT[braceNum]->add_user_type( key.idx, key.idx2, key.len, buf, string_buf,
								type == simdjson::internal::tape_type::START_OBJECT ? 0 : 1, key.id); // object vs array
							key.is_key = false;// ++pool;
						}
						else {
							nestedUT[braceNum]->add_user_type( type == simdjson::internal::tape_type::START_OBJECT ? 0 : 1);
							//++pool;
						}


						class UserType* pTemp = nestedUT[braceNum]->get_data_list(nestedUT[braceNum]->get_data_size() - 1);

						braceNum++;

						/// new nestedUT
						if (nestedUT.size() == braceNum) {
							nestedUT.push_back(nullptr);
						}

						/// initial new nestedUT.
						nestedUT[braceNum] = pTemp;

						state = 0;

					}
					// Right 2
					else if (type == simdjson::internal::tape_type::END_OBJECT ||
						type == simdjson::internal::tape_type::END_ARRAY) {

						if (type == simdjson::internal::tape_type::END_ARRAY && nestedUT[braceNum]->is_object()) {
							std::cout << "{]";
							exit(1);
						}

						if (type == simdjson::internal::tape_type::END_OBJECT && nestedUT[braceNum]->is_array()) {
							std::cout << "[}";
							exit(1);
						}

						state = 0;

						if (!Vec.empty()) {
							if (type == simdjson::internal::tape_type::END_OBJECT) {
								nestedUT[braceNum]->reserve_data_list(nestedUT[braceNum]->get_data_size() + Vec.size() / 2);


								if (Vec.size() % 2 == 1) {
									std::cout << "Vec.size() is odd\n";
									exit(1);
								}


								for (size_t x = 0; x < Vec.size(); x += 2) {
									if (!Vec[x].is_key) {
										std::cout << "is not key\n";
										exit(1);
									}
									if (Vec[x + 1].is_key) {
										std::cout << "is key\n";
										exit(1);
									}

									nestedUT[braceNum]->add_item_type( Vec[x].idx, Vec[x].idx2, Vec[x].len,
										Vec[x + 1].idx, Vec[x + 1].idx2, Vec[x + 1].len, buf, string_buf, Vec[x].id, Vec[x + 1].id);
									//++pool;9

								}
							}
							else { // END_ARRAY
								nestedUT[braceNum]->reserve_data_list(nestedUT[braceNum]->get_data_size() + Vec.size());

								for (auto& x : Vec) {
									if (x.is_key) {

										exit(1);
									}

									nestedUT[braceNum]->add_item_type( (x.idx), x.idx2, x.len, buf, string_buf, x.id);
									//++pool;
								}
							}

							Vec.clear();
						}


						if (braceNum == 0) {
							class UserType ut; //

							ut.add_user_type( type == simdjson::internal::tape_type::END_OBJECT ? 2 : 3); // json -> "var_name" = val  
							//++pool;

							for (size_t i = 0; i < nestedUT[braceNum]->get_data_size(); ++i) {
								ut.get_data_list(0)->add_user_type(nestedUT[braceNum]->get_data_list(i));
								nestedUT[braceNum]->get_data_list(i) = nullptr;
							}

							nestedUT[braceNum]->remove_all();
							nestedUT[braceNum]->add_user_type(ut.get_data_list(0));

							ut.get_data_list(0) = nullptr;

							braceNum++;
						}

						{
							if (braceNum < nestedUT.size()) {
								nestedUT[braceNum] = nullptr;
							}

							braceNum--;
						}
					}
					else {
						{
							Test data;

							data.idx = imple->structural_indexes[token_arr_start + i];
							data.id = token_arr_start + i;

							if (token_arr_start + i + 1 < imple->n_structural_indexes) {
								data.idx2 = imple->structural_indexes[token_arr_start + i + 1];
							}
							else {
								data.idx2 = buf_len;
							}

							bool is_key = false;
							if (token_arr_start + i + 1 < imple->n_structural_indexes && buf[imple->structural_indexes[token_arr_start + i + 1]] == ':') {
								is_key = true;
							}

							if (is_key) {
								data.is_key = true;

								if (token_arr_start + i + 2 < imple->n_structural_indexes) {
									const simdjson::internal::tape_type _type = (simdjson::internal::tape_type)buf[imple->structural_indexes[token_arr_start + i + 2]];

									if (_type == simdjson::internal::tape_type::START_ARRAY || _type == simdjson::internal::tape_type::START_OBJECT) {
										key = std::move(data);
									}
									else {
										Vec.push_back(std::move(data));
									}
								}
								else {
									Vec.push_back(std::move(data));
								}
								++i;

								is_now_comma = false;
								is_before_comma = false;
								//	std::cout << "4-i " << i << "\n";
							}
							else {
								Vec.push_back(std::move(data));
							}

							state = 0;
						}
					}

				}
				break;
				default:
					// syntax err!!
					*err = -1;
					return false; // throw "syntax error ";
					break;
				}
			}

			if (next) {
				*next = nestedUT[braceNum];
			}

			if (Vec.empty() == false) {
				if (Vec[0].is_key) {
					for (size_t x = 0; x < Vec.size(); x += 2) {
						if (!Vec[x].is_key) {
							exit(1);
						}

						if (Vec.size() % 2 == 1) {
							exit(1);
						}


						if (Vec[x + 1].is_key) {
							exit(1);
						}

						nestedUT[braceNum]->add_item_type( Vec[x].idx, Vec[x].idx2, Vec[x].len, Vec[x + 1].idx, Vec[x + 1].idx2, Vec[x + 1].len,
							buf, string_buf, Vec[x].id, Vec[x + 1].id);
						//++pool;
					}
				}
				else {
					for (size_t x = 0; x < Vec.size(); x += 1) {
						if (Vec[x].is_key) {
							exit(1);
						}

						nestedUT[braceNum]->add_item_type( Vec[x].idx, Vec[x].idx2, Vec[x].len, buf, string_buf, Vec[x].id);
						//++pool;
					}
				}

				Vec.clear();
			}

			if (state != last_state) {
				*err = -2;
				return false;
				// throw STRING("error final state is not last_state!  : ") + toStr(state);
			}

			//after_pool = pool;
			int b = clock();
			//std::cout << "parse thread " << b - a << "ms\n";
			return true;
		}

		static int64_t FindDivisionPlace(char* buf, simdjson::internal::dom_parser_implementation* imple, int64_t start, int64_t last)
		{
			for (int64_t a = start; a <= last; ++a) {
				auto& x = imple->structural_indexes[a]; //  token_arr[a];
				const simdjson::internal::tape_type type = (simdjson::internal::tape_type)buf[x];
				bool key = false;
				bool next_is_valid = false;

				switch ((int)type) {
				case ',':
					return a + 1;
				default:
					// error?
					break;
				}
			}
			return -1;
		}
	public:

		static bool _LoadData(class UserType& global, char* buf, size_t buf_len,
			uint8_t* string_buf,
			simdjson::internal::dom_parser_implementation* imple, int64_t& length,
			std::vector<int64_t>& start, const int parse_num) // first, strVec.empty() must be true!!
		{
			int a__ = clock();
			{
				// chk clear?
			
				const int pivot_num = parse_num - 1;
				//size_t token_arr_len = length; // size?

				class UserType _global;

				bool first = true;
				int64_t sum = 0;

				{ int a_ = clock();
				std::set<int64_t> _pivots;
				std::vector<int64_t> pivots;
				//const int64_t num = token_arr_len; //

				if (pivot_num > 0) {
					std::vector<int64_t> pivot;
					pivots.reserve(pivot_num);
					pivot.reserve(pivot_num);

					pivot.push_back(start[0]);

					for (int i = 1; i < parse_num; ++i) {
						pivot.push_back(FindDivisionPlace(buf, imple, start[i], start[i + 1] - 1));
					}

					for (size_t i = 0; i < pivot.size(); ++i) {
						if (pivot[i] != -1) {
							_pivots.insert(pivot[i]);
						}
					}

					for (auto& x : _pivots) {
						pivots.push_back(x);
					}

					pivots.push_back(length);
				}
				else {
					pivots.push_back(start[0]);
					pivots.push_back(length);
				}
				int b_ = clock();
				//std::cout << "pivots.. " << b_ - a_ << "ms\n";
				std::vector<class UserType*> next(pivots.size() - 1, nullptr);
				{

					std::vector<class UserType> __global(pivots.size() - 1);
					for (int i = 0; i < __global.size(); ++i) {
						__global[i].type = -2;
					}


					std::vector<std::thread> thr(pivots.size()-1);

					
					std::vector<int> err(pivots.size() - 1, 0);

					int c1 = clock();
					{
						int64_t idx = pivots[1] - pivots[0];
						int64_t _token_arr_len = idx;


						thr[0] = std::thread(__LoadData, (buf), buf_len, (string_buf), (imple), start[0], _token_arr_len, &__global[0], 0, 0,
							&next[0], &err[0], 0);
						//HANDLE th = thr[0].native_handle();
						//SetThreadPriority(th, THREAD_PRIORITY_HIGHEST);
					}

					for (size_t i = 1; i < pivots.size() - 1; ++i) {
						int64_t _token_arr_len = pivots[i + 1] - pivots[i];

						thr[i] = std::thread(__LoadData, (buf), buf_len, (string_buf), (imple), pivots[i], _token_arr_len, &__global[i], 0, 0,
							&next[i], &err[i], i);

						//HANDLE th = thr[i].native_handle();
						//SetThreadPriority(th, THREAD_PRIORITY_HIGHEST);
					}
					int c2 = clock();
				//	std::cout << "chk... " << c2 - c1 << "\n";

					auto a = std::chrono::steady_clock::now();

					// wait
					for (size_t i = 0; i < thr.size(); ++i) {
						thr[i].join();
					}

					auto b = std::chrono::steady_clock::now();
					auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(b - a);
					std::cout << "parse1 " << dur.count() << "ms\n";

					for (size_t i = 0; i < err.size(); ++i) {
						switch (err[i]) {
						case 0:
							break;
						case -1:
						case -4:
							std::cout << "Syntax Error\n"; return false;
							break;
						case -2:
							std::cout << "error final state is not last_state!\n"; return false;
							break;
						case -3:
							std::cout << "error x > buffer + buffer_len:\n"; return false;
							break;
						default:
							std::cout << "unknown parser error\n"; return false;
							break;
						}
					}

					// Merge
					//try
					{
						int i = 0;
						std::vector<int> chk(parse_num, 0);
						auto x = next.begin();
						auto y = __global.begin();
						while (true) {
							if (y->get_data_size() == 0) {
								chk[i] = 1;
							}

							++x;
							++y;
							++i;

							if (x == next.end()) {
								break;
							}
						}

						int start = 0;
						int last = pivots.size() - 1 - 1;

						for (int i = 0; i < pivots.size() - 1; ++i) {
							if (chk[i] == 0) {
								start = i;
								break;
							}
						}

						for (int i = pivots.size() - 1 - 1; i >= 0; --i) {
							if (chk[i] == 0) {
								last = i;
								break;
							}
						}

						if (__global[start].get_data_size() > 0 && __global[start].get_data_list(0)->is_user_type()
							&& ((UserType*)__global[start].get_data_list(0))->is_virtual()) {
							std::cout << "not valid file1\n";
							throw 1;
						}
						if (next[last] && next[last]->get_parent() != nullptr) {
							std::cout << "not valid file2\n";
							throw 2;
						}



						int err = Merge(&_global, &__global[start], &next[start]);
						if (-1 == err || (pivots.size() == 0 && 1 == err)) {
							std::cout << "not valid file3\n";
							throw 3;
						}

						for (int i = start + 1; i <= last; ++i) {

							if (chk[i]) {
								continue;
							}

							// linearly merge and error check...
							int before = i - 1;
							for (int k = i - 1; k >= 0; --k) {
								if (chk[k] == 0) {
									before = k;
									break;
								}
							}

							int err = Merge(next[before], &__global[i], &next[i]);

							if (-1 == err) {
								std::cout << "chk " << i << " " << __global.size() << "\n";
								std::cout << "not valid file4\n";
								throw 4;
							}
							else if (i == last && 1 == err) {
								std::cout << "not valid file5\n";
								throw 5;
							}
						}
					}
					//catch (...) {
						//throw "in Merge, error";
					//	return false;
					//}
					//

					if (_global.get_data_size() > 1) {
						std::cout << "not valid file6\n";
						throw 6;
					}


					auto c = std::chrono::steady_clock::now();
					auto dur2 = std::chrono::duration_cast<std::chrono::milliseconds>(c - b);
					std::cout << "parse2 " << dur2.count() << "ms\n";
				}
				}
				int a = clock();

				Merge(&global, &_global, nullptr);

				///global = std::move(_global);
				int b = clock();
				//std::cout << "chk " << b - a << "ms\n";

			//	std::cout << clock() - a__ << "ms\n";
			}
		//	std::cout << clock() - a__ << "ms\n";
			return true;

		}
		static bool parse(class UserType& global, char* buf, size_t buf_len,
			uint8_t* string_buf,
			simdjson::internal::dom_parser_implementation* imple,
			int64_t length, std::vector<int64_t>& start, int thr_num) {

			return LoadData::_LoadData(global, buf, buf_len, string_buf, imple, length, start, thr_num);
		}


		class StrStream {
		private:
			fmt::memory_buffer out;
		public:

			const char* buf() const {
				return out.data();
			}
			size_t buf_size() const {
				return out.size();
			}

			StrStream& operator<<(std::string_view x) {
				fmt::format_to(std::back_inserter(out), "{}", x);
				return *this;
			}

			StrStream& operator<<(double x) {
				fmt::format_to(std::back_inserter(out), FMT_COMPILE("{:.10f}"), x);
				return *this;
			}

			StrStream& operator<<(int64_t x) {
				fmt::format_to(std::back_inserter(out), "{}", x);
				return *this;
			}

			StrStream& operator<<(uint64_t x) {
				fmt::format_to(std::back_inserter(out), "{}", x);
				return *this;
			}

			StrStream& operator<<(char ch) {
				fmt::format_to(std::back_inserter(out), "{}", ch);
				return *this;
			}
		};

		//
		static void _save(StrStream& stream, UserType* ut, const int depth = 0) {
			if (!ut) { return; }

			if (ut->is_object()) {
				for (size_t i = 0; i < ut->get_data_size(); ++i) {
					if (ut->get_data_list(i)->is_user_type()) {
						auto& x = ut->get_data_list(i)->value;

						if (
							x.key.type() == simdjson::internal::tape_type::STRING) {
							stream << "\"";
							for (long long j = 0; j < ((std::string&)(*x.key.get_str_val())).size(); ++j) {
								switch ((*x.key.get_str_val())[j]) {
								case '\\':
									stream << "\\\\";
									break;
								case '\"':
									stream << "\\\"";
									break;
								case '\n':
									stream << "\\n";
									break;

								default:
									if (isprint((*x.key.get_str_val())[j]))
									{
										stream << (*x.key.get_str_val())[j];
									}
									else
									{
										int code = (*x.key.get_str_val())[j];
										if (code > 0 && (code < 0x20 || code == 0x7F))
										{
											char buf[] = "\\uDDDD";
											sprintf(buf + 2, "%04X", code);
											stream << buf;
										}
										else {
											stream << (*x.key.get_str_val())[j];
										}
									}
								}
							}

							stream << "\"";

							{
								stream << " : ";
							}
						}
						else {
							std::cout << "Error : no key\n";
						}
						stream << " ";

						if (((UserType*)ut->get_data_list(i))->is_object()) {
							stream << " { \n";
						}
						else {
							stream << " [ \n";
						}

						_save(stream, (UserType*)ut->get_data_list(i), depth + 1);

						if (((UserType*)ut->get_data_list(i))->is_object()) {
							stream << " } \n";
						}
						else {
							stream << " ] \n";
						}
					}
					else {
						auto& x = ut->get_data_list(i)->value;

						if (
							x.key.type() == simdjson::internal::tape_type::STRING) {
							stream << "\"";
							for (long long j = 0; j < (*x.key.get_str_val()).size(); ++j) {
								switch ((*x.key.get_str_val())[j]) {
								case '\\':
									stream << "\\\\";
									break;
								case '\"':
									stream << "\\\"";
									break;
								case '\n':
									stream << "\\n";
									break;

								default:
									if (isprint((*x.key.get_str_val())[j]))
									{
										stream << (*x.key.get_str_val())[j];
									}
									else
									{
										int code = (*x.key.get_str_val())[j];
										if (code > 0 && (code < 0x20 || code == 0x7F))
										{
											char buf[] = "\\uDDDD";
											sprintf(buf + 2, "%04X", code);
											stream << buf;
										}
										else {
											stream << (*x.key.get_str_val())[j];
										}
									}
								}
							}

							stream << "\"";

							{
								stream << " : ";
							}
						}

						{
							auto& x = ut->get_data_list(i)->value;

							if (
								x.data.type() == simdjson::internal::tape_type::STRING) {
								stream << "\"";
								for (long long j = 0; j < ((std::string&)(*x.data.get_str_val())).size(); ++j) {
									switch ((*x.data.get_str_val())[j]) {
									case '\\':
										stream << "\\\\";
										break;
									case '\"':
										stream << "\\\"";
										break;
									case '\n':
										stream << "\\n";
										break;

									default:
										if (isprint((*x.data.get_str_val())[j]))
										{
											stream << (*x.data.get_str_val())[j];
										}
										else
										{
											int code = (*x.data.get_str_val())[j];
											if (code > 0 && (code < 0x20 || code == 0x7F))
											{
												char buf[] = "\\uDDDD";
												sprintf(buf + 2, "%04X", code);
												stream << buf;
											}
											else {
												stream << (*x.data.get_str_val())[j];
											}
										}
									}
								}

								stream << "\"";

							}
							else if (x.data.type() == simdjson::internal::tape_type::TRUE_VALUE) {
								stream << "true";
							}
							else if (x.data.type() == simdjson::internal::tape_type::FALSE_VALUE) {
								stream << "false";
							}
							else if (x.data.type() == simdjson::internal::tape_type::DOUBLE) {
								stream << (x.data.float_val());
							}
							else if (x.data.type() == simdjson::internal::tape_type::INT64) {
								stream << x.data.int_val();
							}
							else if (x.data.type() == simdjson::internal::tape_type::UINT64) {
								stream << x.data.uint_val();
							}
							else if (x.data.type() == simdjson::internal::tape_type::NULL_VALUE) {
								stream << "null ";
							}
						}
					}

					if (i < ut->get_data_size() - 1) {
						stream << ", ";
					}
				}
			}
			else if (ut->is_array()) {
				for (size_t i = 0; i < ut->get_data_size(); ++i) {
					if (ut->get_data_list(i)->is_user_type()) {


						if (((UserType*)ut->get_data_list(i))->is_object()) {
							stream << " { \n";
						}
						else {
							stream << " [ \n";
						}


						_save(stream, (UserType*)ut->get_data_list(i), depth + 1);

						if (((UserType*)ut->get_data_list(i))->is_object()) {
							stream << " } \n";
						}
						else {
							stream << " ] \n";
						}
					}
					else {

						auto& x = ut->get_data_list(i)->value;

						if (
							x.data.type() == simdjson::internal::tape_type::STRING) {
							stream << "\"";
							for (long long j = 0; j < (*x.data.get_str_val()).size(); ++j) {
								switch ((*x.data.get_str_val())[j]) {
								case '\\':
									stream << "\\\\";
									break;
								case '\"':
									stream << "\\\"";
									break;
								case '\n':
									stream << "\\n";
									break;

								default:
									if (isprint((*x.data.get_str_val())[j]))
									{
										stream << (*x.data.get_str_val())[j];
									}
									else
									{
										int code = (*x.data.get_str_val())[j];
										if (code > 0 && (code < 0x20 || code == 0x7F))
										{
											char buf[] = "\\uDDDD";
											sprintf(buf + 2, "%04X", code);
											stream << buf;
										}
										else {
											stream << (*x.data.get_str_val())[j];
										}
									}
								}
							}

							stream << "\"";
						}
						else if (x.data.type() == simdjson::internal::tape_type::TRUE_VALUE) {
							stream << "true";
						}
						else if (x.data.type() == simdjson::internal::tape_type::FALSE_VALUE) {
							stream << "false";
						}
						else if (x.data.type() == simdjson::internal::tape_type::DOUBLE) {
							stream << (x.data.float_val());
						}
						else if (x.data.type() == simdjson::internal::tape_type::INT64) {
							stream << x.data.int_val();
						}
						else if (x.data.type() == simdjson::internal::tape_type::UINT64) {
							stream << x.data.uint_val();
						}
						else if (x.data.type() == simdjson::internal::tape_type::NULL_VALUE) {
							stream << "null ";
						}


						stream << " ";
					}

					if (i < ut->get_data_size() - 1) {
						stream << ", ";
					}
				}
			}
		}

		static void save(const std::string& fileName, class UserType& global) {
			StrStream stream;

			_save(stream, &global);

			std::cout << "Save to StrStream.\n";

			std::ofstream outFile;
			outFile.open(fileName, std::ios::binary); // binary!
			outFile.write(stream.buf(), stream.buf_size());
			outFile.close();
		}

		static void save(std::ostream& stream, class UserType& global) {
			StrStream str_stream;
			_save(str_stream, &global);
			stream << std::string_view(str_stream.buf(), str_stream.buf_size());
		}
	};

	INLINE 	std::pair<bool, size_t> Parse(const std::string& fileName, int thr_num, UserType* ut)
	{
		if (thr_num <= 0) {
			thr_num = std::thread::hardware_concurrency();
		}
		if (thr_num <= 0) {
			thr_num = 1;
		}

		int64_t length;

		int _ = clock();

		{
			simdjson::dom::parser test;

			auto x = test.load(fileName);

			if (x.error() != simdjson::error_code::SUCCESS) {
				std::cout << "stage1 error : ";
				std::cout << x.error() << "\n";

				return { false, 0 };
			}

			if (!test.valid) {
				//	std::cout << "parser is not valid\n";

					//return -2;
			}

			const auto& buf = test.raw_buf();
			const auto& string_buf = test.raw_string_buf();
			const auto& imple = test.raw_implementation();
			const auto buf_len = test.raw_len();

			std::vector<int64_t> start(thr_num + 1, 0);
			//std::vector<int> key;

			int a = clock();

			std::cout << a - _ << "ms\n";


			{
				size_t how_many = imple->n_structural_indexes;
				length = how_many;

				start[0] = 0;
				for (int i = 1; i < thr_num; ++i) {
					start[i] = how_many / thr_num * i;
				}

				int c = clock();


				// no l,u,d  any 
				 // true      true
				 // false     true
				/*
				int count = 1;
				for (; tape_idx < how_many; tape_idx++) {
					if (count < thr_num && tape_idx == start[count]) {
						count++;
					}
					else if (count < thr_num && tape_idx == start[count] + 1) {
						start[count] = tape_idx;
						count++;
					}
					tape_val = tape[tape_idx];
					payload = tape_val & simdjson::internal::JSON_VALUE_MASK;
					type = uint8_t(tape_val >> 56);
					switch (type) {
					case 'l':
					case 'u':
					case 'd':
						tape_idx++;
						break;
					}
				}
				*/

				/*
				int d = clock();
				std::cout << d - c << "ms\n";
				bool now_object = false;
				bool even = false;
				for (tape_idx = 1; tape_idx < how_many; tape_idx++) {
					//os << tape_idx << " : ";
					tape_val = tape[tape_idx];
					payload = tape_val & simdjson::internal::JSON_VALUE_MASK;
					type = uint8_t(tape_val >> 56);
					even = !even;
					switch (type) {
					case '"': // we have a string
						if (now_object && even) {
							key[tape_idx] = 1;
						}
						break;
					case 'l': // we have a long int
					//	if (tape_idx + 1 >= how_many) {
					//		return false;
						//}
						//  os << "integer " << static_cast<int64_t>(tape[++tape_idx]) << "\n";
						++tape_idx;
						break;
					case 'u': // we have a long uint
						//if (tape_idx + 1 >= how_many) {
						//	return false;
						//}
						//  os << "unsigned integer " << tape[++tape_idx] << "\n";
						++tape_idx;
						break;
					case 'd': // we have a double
					  //  os << "float ";
						//if (tape_idx + 1 >= how_many) {
						//	return false;
						//}
						// double answer;
						// std::memcpy(&answer, &tape[++tape_idx], sizeof(answer));
					   //  os << answer << '\n';
						++tape_idx;
						break;
					case 'n': // we have a null
					   // os << "null\n";
						break;
					case 't': // we have a true
					   // os << "true\n";
						break;
					case 'f': // we have a false
					  //  os << "false\n";
						break;
					case '{': // we have an object
					 //   os << "{\t// pointing to next tape location " << uint32_t(payload)
					 //       << " (first node after the scope), "
					  //      << " saturated count "
					   //     << ((payload >> 32) & internal::JSON_COUNT_MASK) << "\n";
						now_object = true; even = false;
						//_stack.push_back(1);
						//_stack2.push_back(0);
						break;
					case '}': // we end an object
					  //  os << "}\t// pointing to previous tape location " << uint32_t(payload)
					  //      << " (start of the scope)\n";
						//_stack.pop_back();
						//_stack2.pop_back();
						now_object = key[uint32_t(payload) - 1] == 1; even = false;
						break;
					case '[': // we start an array
					  //  os << "[\t// pointing to next tape location " << uint32_t(payload)
					  //      << " (first node after the scope), "
					  //      << " saturated count "
					   //     << ((payload >> 32) & internal::JSON_COUNT_MASK) << "\n";
						//_stack.push_back(0);
						now_object = false; even = false;
						break;
					case ']': // we end an array
					 //   os << "]\t// pointing to previous tape location " << uint32_t(payload)
					  //      << " (start of the scope)\n";
						//_stack.pop_back();
						now_object = key[uint32_t(payload) - 1] == 1; even = false;
						break;
					case 'r': // we start and end with the root node
					  // should we be hitting the root node?
						break;
					default:
						return nullptr;
					}
				}
				std::cout << clock() - d << "ms\n";*/
			}


			int b = clock();

			std::cout << b - a << "ms\n";

			start[thr_num] = length;
			if (false == claujson::LoadData::parse(*ut, buf.get(), buf_len, string_buf.get(), imple.get(), length, start, thr_num)) // 0 : use all thread..
			{
				return { false, 0 };
			}
			int c = clock();
			std::cout << c - b << "ms\n";
		}
		int c = clock();
		std::cout << c - _ << "ms\n";

		// claujson::LoadData::_save(std::cout, &ut);

		return  { true, length };
	}

	INLINE int Parse_One(const std::string& str, Data& data) {
		{
			static simdjson::dom::parser test;

			auto x = test.parse(str);

			if (x.error() != simdjson::error_code::SUCCESS) {
				std::cout << x.error() << "\n";

				return -1;
			}

			const auto& buf = test.raw_buf();
			const auto& string_buf = test.raw_string_buf();

			//data = Convert(&tape[1], string_buf);
		}
		return 0;
	}
}