//
// Created by denn nevera on 2019-08-09.
//

#include "KeyChain.h"
#if __has_feature(modules)
@import Foundation;
#else
#import <Foundation/Foundation.h>
#endif

namespace dehancer::platform{

    static constexpr const char* key_chain_id = "com.dehancer.key.chain";

    static Error status_to_error(OSStatus status) {
        if (status != errSecSuccess) {
            NSString *message = @"Security error has occurred.";
            CFStringRef description = SecCopyErrorMessageString(status, NULL);
            if (description) {
                message = (__bridge_transfer NSString *)description;
            }
            return Error(CommonError::SECURITY_ISSUE, [message UTF8String]);
        }

        return Error(dehancer::CommonError::OK);
    }

    NSMutableDictionary* generic_query(const std::string& for_key, const std::string& service_name) {

        NSMutableDictionary *query = [[NSMutableDictionary alloc] init];

        query[(__bridge __strong id)kSecAttrAccount] = [NSString stringWithUTF8String:for_key.c_str()];

        query[(__bridge __strong id)kSecClass]             = (__bridge id)kSecClassGenericPassword;
        query[(__bridge __strong id)kSecAttrGeneric]       = @(key_chain_id);
        query[(__bridge __strong id)(kSecAttrService)]     = [NSString stringWithUTF8String:service_name.c_str()];

        return query;
    }

    KeyChain::KeyChain(const std::string& service_name):
            service_name_(service_name){}

    Error KeyChain::set_object(
            const dehancer::json &object,
            const std::string &for_key, const std::string &label,
            const std::string &comment) {

        NSMutableDictionary* query = generic_query(for_key, service_name_);

        OSStatus status = SecItemCopyMatching((__bridge CFDictionaryRef)query, NULL);

        NSMutableDictionary *attributes = [[NSMutableDictionary alloc] init];

        if (!label.empty()) {
            attributes[(__bridge __strong id)kSecAttrLabel] = [NSString stringWithUTF8String: label.c_str()];
        }
        if (!comment.empty()) {
            attributes[(__bridge __strong id)kSecAttrComment] =[NSString stringWithUTF8String:  comment.c_str()];
        }

        if (floor(NSFoundationVersionNumber) > floor(993.00)) {
            /*
             * TODO: we need to do codesign with right app identity
             *
             * attributes[(__bridge __strong id)kSecAttrAccessible]    = @("ck");
             * attributes[(__bridge __strong id)kSecAttrSynchronizable] = @(YES);
             */
        }

        std::vector<char> value;
        json::to_msgpack(object, value);
        NSData *data = [NSData dataWithBytes: value.data() length:value.size()];

        attributes[(__bridge __strong id)kSecValueData] = data;

        if (status == errSecSuccess || status == errSecInteractionNotAllowed) {
            status = SecItemUpdate((__bridge CFDictionaryRef)query, (__bridge CFDictionaryRef)attributes);
        }
        else  if (status == errSecInteractionNotAllowed && floor(NSFoundationVersionNumber) <= floor(1140.11)) {
            remove_object(for_key);
            status = SecItemAdd((__bridge CFDictionaryRef)query, NULL);
        }
        else  if (status == errSecItemNotFound) {
            for(NSString* key in attributes.allKeys) {
                query[key] = attributes[key];
            }
            status = SecItemAdd((__bridge CFDictionaryRef)query, NULL);
        }

        return status_to_error(status);
    }

    expected<json,Error> KeyChain::get_object(const std::string &for_key) const {

        NSMutableDictionary *query = generic_query(for_key, service_name_);

        query[(__bridge __strong id)kSecMatchLimit] = (__bridge id)kSecMatchLimitOne;
        query[(__bridge __strong id)kSecReturnData] = (__bridge id)kCFBooleanTrue;

        CFTypeRef data = nil;
        OSStatus status = SecItemCopyMatching((__bridge CFDictionaryRef)query, &data);

        if (status == errSecSuccess) {
            NSData *ret = [NSData dataWithData:(__bridge NSData *)data];
            if (data) {

                CFRelease(data);
                const char *buffer = static_cast<const char*>([ret bytes]);
                std::vector<char> value(buffer, buffer+ret.length);

                try {
                    return json::from_msgpack(value);
                }
                catch (std::exception& e) {
                    return make_unexpected(Error(CommonError::EXCEPTION, e.what()));
                }

            } else {
                return make_unexpected(Error(CommonError::NOT_FOUND, "Unexpected error has occurred."));
            }
        } else if (status == errSecItemNotFound) {
            return make_unexpected(Error(CommonError::NOT_FOUND, "Item not found"));
        }

        return make_unexpected(status_to_error(status));
    }

    Error KeyChain::remove_object(const std::string &for_key) {
        NSMutableDictionary *query = [[NSMutableDictionary alloc] init];
        query[(__bridge __strong id)kSecAttrAccount] = [NSString stringWithUTF8String:for_key.c_str()];
        return status_to_error(SecItemDelete((__bridge CFDictionaryRef)query));
    }
}