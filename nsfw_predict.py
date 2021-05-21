#-*-coding:utf-8-*-

import os
import io
import sys
import magic
from raids_h import rehis_zero_pic,rehis_zero
import numpy as np
from PIL import Image
import tensorflow as tf
import base64,time
from config import config_h
_MODEL_DIR = os.path.join(os.path.abspath(os.path.dirname(__file__)), 'data/models/1547856517')
_IMAGE_SIZE = 64
_BATCH_SIZE = 128

_LABEL_MAP = {0:'drawings', 1:'hentai', 2:'neutral', 3:'porn', 4:'sexy'}

def standardize(img):
    mean = np.mean(img)
    std = np.std(img)
    img = (img - mean) / std
    return img


def load_image(data):
    # img = Image.open( data )
    img = base64.b64decode(data)
    img= Image.open(io.BytesIO(img))
    img = img.convert('RGB')
    img = img.resize((_IMAGE_SIZE, _IMAGE_SIZE))
    img.load()
    data = np.asarray( img, dtype=np.float32 )
    data = standardize(data)
    print('load_image')
    return data

def predict(image_date):
    with tf.Session() as sess:
        print('******')
        graph = tf.get_default_graph();
        tf.saved_model.loader.load(sess, [tf.saved_model.tag_constants.SERVING], _MODEL_DIR)
        inputs = graph.get_tensor_by_name("input_tensor:0")
        probabilities_op = graph.get_tensor_by_name('softmax_tensor:0')
        class_index_op = graph.get_tensor_by_name('ArgMax:0')
        image_data = load_image(image_date)
        probabilities, class_index = sess.run([probabilities_op, class_index_op],
                                              feed_dict={inputs: [image_data] * _BATCH_SIZE})
        probabilities_dict = {_LABEL_MAP.get(i): l for i, l in enumerate(probabilities[0])}
        pre_label = _LABEL_MAP.get(class_index[0])
        result = {"class": pre_label, "probability": probabilities_dict}
        return result

ckeck_key = config_h.get_config('check','check_key')
if __name__ == '__main1__':
    while True:
        url = rehis_zero_pic.get('pic_check')
        if url:
            rehis_zero_pic.delete('pic_check')
            count=rehis_zero_pic.hlen(url)
            images=rehis_zero_pic.hscan(url,cursor=0,count=count)
            s = 0
            # 图片识别类型
            # {'drawings', 'hentai', 'neutral', 'porn','sexy'}
            for k,v in images[1].items():
                print(v)
                res = predict(v)
                print(res)
                pic_type=res['class']
                rehis_zero_pic.set(pic_type, k)
        time.sleep(1)

if __name__ == '__main1__':
    argv = sys.argv
    if(len(argv) < 2):
        print("usage: python nsfw_predict <image_path>")
    image_path = argv[1]
    print()
    res = predict(image_path)
    print(res)

ckeck_key = config_h.get_config('check','check_key')
check_key = ckeck_key
from tool.check_file import check_file
if __name__ == '__main__':
    while True:
        s=rehis_zero.hscan(check_key,cursor=0,count=10)
        # s = rehis_zero.hscan_iter(check_key,count=1)
        # print(s)
        for i in s[1]:
            key = i.decode('utf-8')
            check_list=key.split(':')
            if check_list[1] == 'image':
                try:
                    v=rehis_zero_pic.hget(check_key, key)
                    rehis_zero_pic.hdel(check_key,key)
                    # print('可以转utf-8',key)
                    res = predict(v)
                    if res['class'] == 'porn' or res['class'] == 'hentai':
                        # print('这个图片是色情图片')
                        rehis_zero_pic.hset('pron',key,v)
                        rehis_zero_pic.hdel(check_key, key)
                    else:
                        rehis_zero_pic.hset('normal', key, v)
                        rehis_zero_pic.hdel(check_key,key)
                    # print(res,check_key)
                except Exception as e:
                    print(key, ':', e)
            elif check_list[1] == 'text':
                s = rehis_zero.hget(check_key,key)
                # rehis_zero_pic.hdel(check_key,key)
                try:
                    file_title=s[0:2048]
                    file_type=magic.from_buffer(file_title)
                    print(file_type)
                    if 'text' in file_type:
                        print('这个是text:', file_type)
                        print(type(s))
                        # print(s.decode('utf-8'))
                        with open(f'tmp/{check_list[2]}','wb') as f:
                            f.write(s)
                        score=check_file(file=f'tmp/{check_list[2]}')
                        print(score,type(score))
                        score=int(score)
                        if score > 10:
                            rehis_zero.hset('porn',key,s)

                        else:
                            rehis_zero.hset('normal', key, s)
                        rehis_zero.hdel(check_key, key)
                    elif 'Word' in file_type:
                        with open(f'tmp/{check_list[2]}', 'wb') as f:
                            f.write(s)
                        score = check_file(file=f'tmp/{check_list[2]}')
                        print(score, type(score))
                        score = int(score)
                        if score > 10:
                            rehis_zero.hset('porn', key, s)
                        else:
                            rehis_zero.hset('normal', key, s)
                        rehis_zero.hdel(check_key, key)
                    elif 'at least v1.0 to extract' in file_type:
                        with open(f'tmp/{check_list[2]}', 'wb') as f:
                            f.write(s)
                        score = check_file(file=f'tmp/{check_list[2]}')
                        print(score, type(score))
                        score = int(score)
                        if score > 10:
                            rehis_zero.hset('porn', key, s)
                        else:
                            rehis_zero.hset('normal', key, s)
                        rehis_zero.hdel(check_key, key)

                    else:
                        print('未知类型',key)
                        rehis_zero.hdel(check_key, key)
                except Exception as e:
                    print('未知原因',e)
            elif check_list[1] == 'video':
                print('删除视频文件:',key)
                rehis_zero.hdel(check_key, key)

            else:
                rehis_zero.hdel(check_key, key)
